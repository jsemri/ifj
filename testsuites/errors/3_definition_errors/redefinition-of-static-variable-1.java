// viz https://wis.fit.vutbr.cz/FIT/st/phorum-msg-show.php?id=45741
class Main {
    static int a = b;
    static int b = a;
    static void run() {
        a = a + b;
        b = a + b;
    }
}