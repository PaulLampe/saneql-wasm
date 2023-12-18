#include "algebra/Operator.hpp"
#include "infra/Schema.hpp"
#include "parser/ASTBase.hpp"
#include "parser/SaneQLLexer.hpp"
#include "parser/SaneQLParser.hpp"
#include "semana/SemanticAnalysis.hpp"
#include "sql/SQLWriter.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <emscripten.h>
#include <emscripten/bind.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace saneql;
//---------------------------------------------------------------------------
// (c) 2023 Thomas Neumann
//---------------------------------------------------------------------------
static string readFiles(unsigned count, char* files[]) {
   ostringstream output;
   for (unsigned i = 0; i != count; i++) {
      ifstream in(files[i]);
      if (!in.is_open()) {
         cerr << "unable to read " << files[i] << endl;
         exit(1);
      }
      output << in.rdbuf();
      output << "\n";
   }
   return output.str();
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
   if (argc < 2) {
      cerr << "usage: " << argv[0] << " file..." << endl;
      return 1;
   }

   Schema schema;
   schema.populateSchema();

   string query = readFiles(argc - 1, argv + 1);
   ASTContainer container;
   ast::AST* tree = nullptr;
   try {
      tree = SaneQLParser::parse(container, query);
   } catch (const exception& e) {
      cerr << e.what() << endl;
      return 1;
   }

   SemanticAnalysis semana(schema);
   try {
      auto res = semana.analyzeQuery(tree);
      SQLWriter sql;
      if (res.isScalar()) {
         sql.write("select ");
         res.scalar()->generate(sql);
      } else {
         algebra::Sort* sort = nullptr;
         auto tree = res.table().get();
         if (auto s = dynamic_cast<algebra::Sort*>(tree)) {
            sort = s;
            tree = sort->input.get();
         }
         sql.write("select ");
         bool first = true;
         for (auto& c : res.getBinding().getColumns()) {
            if (first)
               first = false;
            else
               sql.write(", ");
            sql.writeIU(c.iu);
            sql.write(" as ");
            sql.writeIdentifier(c.name);
         }
         sql.write(" from ");
         tree->generate(sql);
         sql.write(" s");
         if (sort) {
            if (!sort->order.empty()) {
               sql.write(" order by ");
               bool first = true;
               for (auto& o : sort->order) {
                  if (first)
                     first = false;
                  else
                     sql.write(", ");
                  o.value->generate(sql);
                  if (o.collate != Collate{}) sql.write(" collate TODO"); // TODO
                  if (o.descending) sql.write(" desc");
               }
            }
            if (sort->limit.has_value()) {
               sql.write(" limit ");
               sql.write(to_string(*(sort->limit)));
            }
            if (sort->offset.has_value()) {
               sql.write(" offset ");
               sql.write(to_string(*(sort->offset)));
            }
         }
      }
      cout << sql.getResult() << endl;
   } catch (const exception& e) {
      cerr << e.what() << endl;
      return 1;
   }

   return 0;
}

std::string char_to_hex(char c) {
    std::stringstream ss;
    ss << std::hex << (int)c;
    std::string result = ss.str();
    return result.size() == 1 ? "000" + result : "00" + result;
}

std::string escape_string_for_json(const std::string& input) {
    std::string output;
    output.reserve(input.length()); // reserve memory to improve performance

    for (char c : input) {
        switch (c) {
            case '\"': output += "\\\""; break; // Escape double-quote
            case '\\': output += "\\\\"; break; // Escape backslash
            case '\b': output += "\\b";  break; // Escape backspace
            case '\f': output += "\\f";  break; // Escape formfeed
            case '\n': output += "\\n";  break; // Escape newline
            case '\r': output += "\\r";  break; // Escape carriage return
            case '\t': output += "\\t";  break; // Escape tab
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    // Escape other control characters
                    output += "\\u" + char_to_hex(c);
                } else {
                    output += c;
                }
        }
    }

    return output;
}

EMSCRIPTEN_KEEPALIVE
std::string saneql_to_sql(std::string q) {
   Schema schema;
   schema.populateSchema();

   string query = std::string(q);

   ASTContainer container;

   ast::AST* tree = nullptr;
   try {
      tree = SaneQLParser::parse(container, query);
   } catch (const exception& e) {
      cerr << e.what() << endl;
      return std::string("{ \"error\": \"" + escape_string_for_json(std::string(e.what())) + "\", \"query\": \"\", \"input\": \"" + q + "\" }");
   }

   SemanticAnalysis semana(schema);
   try {
      auto res = semana.analyzeQuery(tree);
      SQLWriter sql;
      if (res.isScalar()) {
         sql.write("select ");
         res.scalar()->generate(sql);
      } else {
         algebra::Sort* sort = nullptr;
         auto tree = res.table().get();
         if (auto s = dynamic_cast<algebra::Sort*>(tree)) {
            sort = s;
            tree = sort->input.get();
         }
         sql.write("select ");
         bool first = true;
         for (auto& c : res.getBinding().getColumns()) {
            if (first)
               first = false;
            else
               sql.write(", ");
            sql.writeIU(c.iu);
            sql.write(" as ");
            sql.writeIdentifier(c.name);
         }
         sql.write(" from ");
         tree->generate(sql);
         sql.write(" s");
         if (sort) {
            if (!sort->order.empty()) {
               sql.write(" order by ");
               bool first = true;
               for (auto& o : sort->order) {
                  if (first)
                     first = false;
                  else
                     sql.write(", ");
                  o.value->generate(sql);
                  if (o.collate != Collate{}) sql.write(" collate TODO"); // TODO
                  if (o.descending) sql.write(" desc");
               }
            }
            if (sort->limit.has_value()) {
               sql.write(" limit ");
               sql.write(to_string(*(sort->limit)));
            }
            if (sort->offset.has_value()) {
               sql.write(" offset ");
               sql.write(to_string(*(sort->offset)));
            }
         }
      }
      return std::string("{ \"error\": \"\", \"query\": \"" + escape_string_for_json(sql.getResult()) + "\", \"input\": \"" + q + "\" }");
   } catch (const exception& e) {
      cerr << e.what() << endl;
      return std::string("{ \"error\": \"" + escape_string_for_json(std::string(e.what())) + "\", \"query\": \"\", \"input\": \"" + q + "\" }");
   }

      return "";
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("saneql_to_sql", &saneql_to_sql);
}