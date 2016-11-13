class Main {
	static double a;
    static void run() {
        static int foo(){
        	a = 5.6;
        	return a; // returning double in int function
        }
    }
}