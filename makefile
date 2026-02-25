OPTIONS = -framework GLUT -framework OpenGL
DEFINES = -D GL_SILENCE_DEPRECATION

triangle: triangle.c initShader.o
	gcc -o triangle triangle.c initShader.o $(OPTIONS) $(DEFINES)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c $(DEFINES)

myLib.o: myLib.c myLib.h
	gcc -c myLib.c $(DEFINES)

clean:
	rm *.o
	rm triangle
