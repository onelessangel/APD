Copyright Teodora Stroe 331CA 2022

# Black Friday Order Manager

## Entities

The program uses:

- **OrderTask** threads as level 1 threads, handling the orders;
- **ProductTask** threads as level 2 threads, handling the products.

The starting point of the program is the Tema2 class.

## Workflow

- *P* **orderTask** threads are started, sharing the same buffered orderReader/Writer and productsWriter.
- Each **orderTask** thread:
	- Reads one line from the "orders.txt" file in a synchronized block;
	- Extracts the order id and the number of products;
	- Instantiates an atomic counter with the number of products in the order;
	- Starts a number of productThread threads equal to the number of products in the order.
- Each **productTask** thread:
	- Using a shared buffered reader, reads lines from the "order_products.txt" file in a synchronized block as long as there are still products from the respective order to be processed;
	- Extracts the id of the current order and, if it is not the same as the one searched for, moves on to the next line.
	- After it marks the product as "shipped", it decrements the products counter and exits, leaving the other products to be processed by their own threads.
- A semaphore is used to limit the number of level 2 threads to *P*.
- Writing in files is also done in a synchronized block.

## Resources

[1] [Laboratorul 4 - Introducere în Java Multithreading](https://ocw.cs.pub.ro/courses/apd/laboratoare/04)

[2] [Laboratorul 6 - Structuri și operații atomice în Java](https://ocw.cs.pub.ro/courses/apd/laboratoare/06)