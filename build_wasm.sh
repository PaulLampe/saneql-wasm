mkdir -p bin/parser/
emcc -c -obin/parser/ASTBase.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/parser/ASTBase.d parser/ASTBase.cpp
mkdir -p bin/parser/
emcc -c -obin/parser/SaneQLLexer.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/parser/SaneQLLexer.d parser/SaneQLLexer.cpp
mkdir -p bin/infra/
emcc -c -obin/infra/Schema.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/infra/Schema.d infra/Schema.cpp
mkdir -p bin/semana/
emcc -c -obin/semana/Functions.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/semana/Functions.d semana/Functions.cpp
mkdir -p bin/parser/
emcc -c -obin/parser/ASTBase.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/parser/ASTBase.d parser/ASTBase.cpp
mkdir -p bin/parser/
emcc -c -obin/parser/SaneQLLexer.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/parser/SaneQLLexer.d parser/SaneQLLexer.cpp
mkdir -p bin/infra/
emcc -c -obin/infra/Schema.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/infra/Schema.d infra/Schema.cpp
mkdir -p bin/semana/
emcc -c -obin/semana/Functions.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/semana/Functions.d semana/Functions.cpp
mkdir -p bin/makeutil/
g++ -c -obin/makeutil/astgen.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/makeutil/astgen.d makeutil/astgen.cpp
g++ -std=c++23 -Ibin/ -I. -g -Wall -Wextra -obin/astgen bin/makeutil/astgen.o
mkdir -p bin/parser/
bin/astgen --astbaseheader parser/ASTBase.hpp --astbaseclass saneql::ASTBase --astheader parser/AST.hpp --namespace saneql::ast astheader parser/astspec bin/parser/AST.hpp
mkdir -p bin/semana/
emcc -c -obin/semana/SemanticAnalysis.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/semana/SemanticAnalysis.d semana/SemanticAnalysis.cpp
mkdir -p bin/algebra/
emcc -c -obin/algebra/Expression.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/algebra/Expression.d algebra/Expression.cpp
mkdir -p bin/algebra/
emcc -c -obin/algebra/Operator.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/algebra/Operator.d algebra/Operator.cpp
mkdir -p bin/sql/
emcc -c -obin/sql/SQLWriter.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/sql/SQLWriter.d sql/SQLWriter.cpp
mkdir -p bin/
emcc -c -obin/main.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/main.d main.cpp
mkdir -p bin/parser/
bin/astgen --astbaseheader parser/ASTBase.hpp --astbaseclass saneql::ASTBase --astheader parser/AST.hpp --namespace saneql::ast expandgrammar parser/astspec parser/saneql.ypp bin/parser/saneql.expanded.ypp
makeutil/patchbison `which bison` bin/parser/saneql.expanded.ypp bin/parser/saneql_parser.cpp
mkdir -p bin/parser/
emcc -c -obin/parser/saneql_parser.o -std=c++23 -Ibin/ -I. -g -Wall -Wextra -MMD -MP -MF bin/parser/saneql_parser.d bin/parser/saneql_parser.cpp
emcc --no-entry -lembind -fexceptions -std=c++23 -Ibin/ -I. -g -Wall -Wextra -obin/saneql.mjs bin/parser/ASTBase.o bin/parser/SaneQLLexer.o bin/infra/Schema.o bin/semana/Functions.o bin/semana/SemanticAnalysis.o bin/algebra/Expression.o bin/algebra/Operator.o bin/sql/SQLWriter.o bin/main.o bin/parser/saneql_parser.o \
      --pre-js=locateFiles.js \
      -s ENVIRONMENT='web'  \
	  -s EXPORT_NAME='createModule'  \
	  -s USE_ES6_IMPORT_META=0  \
	  -s EXPORTED_FUNCTIONS='[]'  \
	  -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap","stringToNewUTF8"]'