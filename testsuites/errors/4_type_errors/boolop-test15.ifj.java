class Main {
	static int a;
	
    static void run() {
        foo();
    }
    
    static boolean foo(){
    	a = 5;
    	return a; // returning int in boolean function
    }
}
