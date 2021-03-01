SRC=src/
TST=test/

all: myc

y.tab.h y.tab.c: $(SRC)lang.y
	bison -v -y -d $(SRC)lang.y

lex.yy.c:	$(SRC)lang.l y.tab.h
	flex $(SRC)lang.l

myc: lex.yy.c y.tab.c $(SRC)Table_des_symboles.c $(SRC)Table_des_chaines.c $(SRC)Attribute.c $(SRC)Production_de_code.c
	gcc -o myc lex.yy.c y.tab.c $(SRC)Attribute.c $(SRC)Table_des_symboles.c $(SRC)Table_des_chaines.c $(SRC)Production_de_code.c

test: myc
	./myc < $(TST)test.myc

clean:
	rm -f 	lex.yy.c *.o y.tab.h y.tab.c myc *~ y.output $(TST)test.c $(TST)test.h ${TST}test



