class Main {
	static int a;
    static void run() {
        static boolean foo(){
        	a = 5;
        	return a; // returning int in boolean function
        }
    }
}