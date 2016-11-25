/**
 * 8 dam
 *
 * Najde všechny způsoby, jak do šachovnice umístit co nejvyšší počet (8) dam
 * tak, aby se vzájemně neohrožovaly.
 *
 * Inspirováno poslední přednáškou z IAL.
 */
 
class Main {
    static void run() {
        zkus(0, 0, 0, 0, 0, 0, 0, 0);
    }
    
    static void zkus(int s1, int s2, int s3, int s4, int s5, int s6, int s7,
                     int s8) {
        int i = 1;
        int jeProblem;
        
        if (s1 == 0) {
            while (i <= 8) {
                zkus(i, s2, s3, s4, s5, s6, s7, s8);
                i = i + 1;
            }
            return;
        }
        
        if (s2 == 0) {
            while (i <= 8) {
                zkus(s1, i, s3, s4, s5, s6, s7, s8);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s2, 1);if (jeProblem > 0) {return;}
        
        if (s3 == 0) {
            while (i <= 8) {
                zkus(s1, s2, i, s4, s5, s6, s7, s8);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s3, 2);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s2, s3, 1);if (jeProblem > 0) {return;}
        
        if (s4 == 0) {
            while (i <= 8) {
                zkus(s1, s2, s3, i, s5, s6, s7, s8);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s4, 3);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s2, s4, 2);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s3, s4, 1);if (jeProblem > 0) {return;}
        
        if (s5 == 0) {
            while (i <= 8) {
                zkus(s1, s2, s3, s4, i, s6, s7, s8);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s5, 4);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s2, s5, 3);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s3, s5, 2);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s4, s5, 1);if (jeProblem > 0) {return;}
        
        if (s6 == 0) {
            while (i <= 8) {
                zkus(s1, s2, s3, s4, s5, i, s7, s8);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s6, 5);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s2, s6, 4);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s3, s6, 3);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s4, s6, 2);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s5, s6, 1);if (jeProblem > 0) {return;}
        
        if (s7 == 0) {
            while (i <= 8) {
                zkus(s1, s2, s3, s4, s5, s6, i, s8);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s7, 6);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s2, s7, 5);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s3, s7, 4);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s4, s7, 3);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s5, s7, 2);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s6, s7, 1);if (jeProblem > 0) {return;}
        
        if (s8 == 0) {
            while (i <= 8) {
                zkus(s1, s2, s3, s4, s5, s6, s7, i);
                i = i + 1;
            }
            return;
        }
        jeProblem = vadiSi(s1, s8, 7);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s2, s8, 6);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s3, s8, 5);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s4, s8, 4);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s5, s8, 3);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s6, s8, 2);if (jeProblem > 0) {return;}
        jeProblem = vadiSi(s7, s8, 1);if (jeProblem > 0) {return;}
        
        
        nakresli(s1, s2, s3, s4, s5, s6, s7, s8);
    }
    
    static int vadiSi(int s1, int s2, int rozdil) {
        if (s1 == s2) {
            return 1;
        }
        if (s1 - s2 == rozdil) {
            return 1;
        }        
        if (s2 - s1 == rozdil) {
            return 1;
        }
        return 0;
    }
    
    static int counter = 1;
    static void nakresli(int s1, int s2, int s3, int s4, int s5, int s6, int s7,
                         int s8) {
        ifj16.print("\nMožnost č. " + counter + "\n  A|B|C|D|E|F|G|H|\n");
        int radek = 1;
        while (radek <= 8) {
            nakrRadek(radek, s1, s2, s3, s4, s5, s6, s7, s8);
            radek = radek + 1;
        }
        
        counter = counter + 1;
    }
    
    static void nakrRadek(int radek, int s1, int s2, int s3, int s4, int s5,
                          int s6, int s7, int s8) {
        String radekStr = radek + " ";
        String c1 = " |"; if (s1 == radek) {c1 = "X|";}
        String c2 = " |"; if (s2 == radek) {c2 = "X|";}
        String c3 = " |"; if (s3 == radek) {c3 = "X|";}
        String c4 = " |"; if (s4 == radek) {c4 = "X|";}
        String c5 = " |"; if (s5 == radek) {c5 = "X|";}
        String c6 = " |"; if (s6 == radek) {c6 = "X|";}
        String c7 = " |"; if (s7 == radek) {c7 = "X|";}
        String c8 = " |"; if (s8 == radek) {c8 = "X|";}
        ifj16.print(radekStr + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + "\n");
    }
}

