CC=gcc
CFLAGS=-ansi -Wall -pedantic -g
DEPENDENCIES=error.o reader.o utils.o parser.o writer.o  symbol_table.o macro_table.o translator.o image.o lexer.o
DRIVER=assembler

$(DRIVER): $(DEPENDENCIES) main.c main.h
	$(CC) $(CFLAGS) $(DEPENDENCIES) main.c -o $(DRIVER)
	
error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o error.o
	
reader.o: reader.c reader.h
	$(CC) $(CFLAGS) -c reader.c -o reader.o
	
parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o
	
writer.o: writer.c writer.h
	$(CC) $(CFLAGS) -c writer.c -o writer.o
	
utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o
	
symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c -o symbol_table.o
	
macro_table.o: macro_table.c macro_table.h
	$(CC) $(CFLAGS) -c macro_table.c -o macro_table.o
	
translator.o: translator.c translator.h
	$(CC) $(CFLAGS) -c translator.c -o translator.o
	
image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c -o image.o
	
lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c -o lexer.o

	
clean:
	rm $(DRVIER) $(DEPENDENCIES)