class Main {
    static String b;
    static void run() {
        foo.c = 4;
        foo.b(b);   // expect int
    }
}

class foo {
    static int c;
    static void b(int c);
}
