class Main {
	static double a;
    static void run() {
        static boolean foo(){
        	a = 5.5;
        	return a; // returning double in boolean function
        }
    }
}