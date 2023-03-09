run: build
	java Tema2 ../checker/sample_data/input/input_a 2

build: Tema2

Tema2: Tema2.java OrderTask.java ProductTask.java
		javac $^

clean:
		rm -f *.class