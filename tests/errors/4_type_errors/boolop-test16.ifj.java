class Main {
	static double a;
	
    static void run() {
        foo();
    }
    
    static boolean foo(){
    	a = 5.5;
    	return a; // returning double in boolean function
    }
}
