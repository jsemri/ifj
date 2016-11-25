class Main {
    static void run() {
        Utils.p(a);
        int a = 10;
        Utils.p(a);
        Utils.p(Main.a);
        Utils.p(OtherClass.a);
        OtherClass.foo(a);
        Utils.p(a);
        Utils.p(Main.a);
    }
    static int a = 5;
}

class OtherClass {
    static int a = 3;
    static void foo(int a) {
        Main.a = 99;
        Utils.p(a);
        a = 20;
        Utils.p(Main.a);
        Utils.p(OtherClass.a);
    }
}

class Utils {
    static int counter = 1;
    static void p(int a) {
        ifj16.print("[" + counter + "] V áčku je: " + a + "\n");
        counter = counter + 1;
    }
}
