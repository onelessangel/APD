import java.io.*;
import java.util.concurrent.atomic.AtomicInteger;

public class OrderTask implements Runnable{
    BufferedReader productsReader;
    BufferedReader br;
    BufferedWriter bw;
    BufferedWriter pw;
    String processedOrder = null;
    String orderId;
    int noProducts;
    AtomicInteger productsCounter;

    public OrderTask(BufferedReader br, BufferedWriter bw, BufferedWriter pw) {
        this.br = br;
        this.bw = bw;
        this.pw = pw;
    }

    @Override
    public void run() {
        while (true) {
            synchronized (this) {
                try {
                    if (!br.ready()) {
                        break;
                    }

                    processedOrder = br.readLine();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            orderId = processedOrder.split(",")[0];
            noProducts = Integer.parseInt(processedOrder.split(",")[1]);

            if (noProducts == 0) {
                continue;
            }

            productsCounter = new AtomicInteger(noProducts);

            Thread[] t = new Thread[noProducts];

            try {
                productsReader = new BufferedReader(new FileReader(Tema2.inputFolder + "/order_products.txt"));

                for (int i = 0; i < noProducts; i++) {
                    t[i] = new Thread(new ProductTask(productsReader, pw, productsCounter, orderId));
                    t[i].start();
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }

            for (int i = 0; i < noProducts; i++) {
                try {
                    t[i].join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            synchronized (this) {
                try {
                    bw.write(processedOrder + ",shipped\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            try {
                productsReader.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
