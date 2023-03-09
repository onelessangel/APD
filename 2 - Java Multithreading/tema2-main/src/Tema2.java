import java.io.*;
import java.util.concurrent.Semaphore;

public class Tema2 {
    static String inputFolder;
    static int noThreads;
    static Semaphore semaphore;

    public static void main(String[] args) throws IOException {
        inputFolder = args[0];
        noThreads = Integer.parseInt(args[1]);
        semaphore = new Semaphore(noThreads);

        BufferedReader orderReader = new BufferedReader(new FileReader(inputFolder + "/orders.txt"));

        BufferedWriter orderWriter = new BufferedWriter(new FileWriter("orders_out.txt"));
        BufferedWriter productsWriter = new BufferedWriter(new FileWriter("order_products_out.txt"));

        Thread[] t = new Thread[noThreads];

        for (int i = 0; i < noThreads; i++) {
            t[i] = new Thread(new OrderTask(orderReader, orderWriter, productsWriter));
            t[i].start();
        }

        for (int i = 0; i < noThreads; i++) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        orderReader.close();
        orderWriter.close();
        productsWriter.close();
    }
}
