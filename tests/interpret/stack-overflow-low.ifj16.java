class Main {
    static void run() {
        foo(5000);
        ifj16.print("\n");
    }
    static void foo(int c) {
        if (c > 0) {
            c = c - 1;
            ifj16.print(c + " ");
            foo(c);
        }
    }
}
