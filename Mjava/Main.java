package Mjava;

class NumRand implements Runnable {
    
    private String threadName;
    
    NumRand(String name) {
        this.threadName = name;
    } 
    public void run() {
        for (int i = 1; i <= 5; i++) {
            int randomNum = (int)(Math.random() * 100); // 0 to 99
            System.out.println(threadName + " | Round " + i + " → Random Number: " + randomNum);
            
            try {
                Thread.sleep(500); // pause 0.5 sec between each number
            } catch (InterruptedException e) {
                System.out.println(threadName + " was interrupted.");
            }
        }
        System.out.println(threadName + " DONE.");
    }
}
class Main {
   Main() {
   }
   public static void main(String[] var0) {
      NumRand var1 = new NumRand("Thread-1");
      NumRand var2 = new NumRand("Thread-2");
      NumRand var3 = new NumRand("Thread-3");
      Thread var4 = new Thread(var1);
      Thread var5 = new Thread(var2);
      Thread var6 = new Thread(var3);
      var4.start();
      var5.start();
      var6.start();
   }
}
