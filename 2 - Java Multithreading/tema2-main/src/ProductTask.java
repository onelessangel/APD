import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicInteger;

public class ProductTask implements Runnable{
    BufferedReader br;
    BufferedWriter pw;
    AtomicInteger counter;
    String processedProduct;
    String orderId;
    String currentOrderId;
    String productId;

    public ProductTask(BufferedReader br, BufferedWriter pw, AtomicInteger counter, String orderId) {
        this.br = br;
        this.pw = pw;
        this.counter = counter;
        this.orderId = orderId;
    }

    @Override
    public void run() {
        try {
            Tema2.semaphore.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        while (counter.get() != 0) {
            synchronized (this) {
                try {
                    if (!br.ready()) {
                        break;
                    }

                    processedProduct = br.readLine();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            currentOrderId = processedProduct.split(",")[0];
            productId = processedProduct.split(",")[1];

            if (!currentOrderId.equals(orderId)) {
                continue;
            }

            synchronized (this) {
                try {
                    pw.write(processedProduct + ",shipped\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            counter.decrementAndGet();
            break;
        }

        Tema2.semaphore.release();
    }
}
