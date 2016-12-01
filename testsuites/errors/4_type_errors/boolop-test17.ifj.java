class Main {
	static String a;
    
    static void run() {
        foo();
    }
    
    static boolean foo(){
    	a = "str";
    	return a; // returning string in boolean function
    }
}
