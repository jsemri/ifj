class Main {
	static String a;
    static void run() {
        static int foo(){
        	a = "string";
        	return a; // returning string in int function
        }
    }
}