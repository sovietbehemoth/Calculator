
compile:
	@gcc main.c -lm src/tokenizer.c src/lexer.c src/parser.c src/visitor.c -o Calculator
