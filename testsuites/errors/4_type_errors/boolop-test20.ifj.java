class Main {
    static void run() {
        sec.run("str");   // calling with String instead of boolean parameter
    }
}

class sec {
    static String run(boolean a) {
        String c = "str";
        return c; 
    }
}