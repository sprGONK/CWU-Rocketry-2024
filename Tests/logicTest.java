package Tests;
public class logicTest{
    public static String convertIntegerToBinary(int n) {
        if (n == 0) {
            return "0";
        }
        StringBuilder binaryNumber = new StringBuilder();
        while (n > 0) {
            int remainder = n % 2;
            binaryNumber.append(remainder);
            n /= 2;
        }
        binaryNumber = binaryNumber.reverse();
        return binaryNumber.toString();
    }

    public static void main(String args[]){
        int altitude = 0, ignite = 0, apogee=0;
        while(1==1){
            //ascending
            if(altitude<=5000&&apogee!=1){
                altitude += 50;
                if(altitude>=5000){//apogee reached
                    apogee = 1;
                }
            }
            //falling back down
            if(apogee==1){
                altitude-=50;
                if(altitude == 0){
                    return;//we've landed, finish
                }
            }
            //received permission
            if(altitude == 4500){
                ignite |= 1;
            }
            //logic checks
            if(altitude > 1000){
                ignite |= 4; //above 1000ft? set bit 3 high
            }
            if(ignite >= 4){ // are bits 1 and or 3 high? proceed
                if(altitude < (550) && altitude > (400)){
                    ignite |= 2; // in ejection altitude range? set bit 2 high
                }
            }
            System.out.println("Altitude = "+altitude+". Ignite command = "+convertIntegerToBinary(ignite));
        }
    }
}