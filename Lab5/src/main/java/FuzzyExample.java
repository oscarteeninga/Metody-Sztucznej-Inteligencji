import net.sourceforge.jFuzzyLogic.FIS;
import net.sourceforge.jFuzzyLogic.rule.FuzzyRuleSet;

public class FuzzyExample {

    public static void main(String[] args) throws Exception {
        try {
            String fileName = args[0];
            int poziomNatezenia = Integer.parseInt(args[1]);
            int poraDnia = Integer.parseInt(args[2]);
            FIS fis = FIS.load(fileName,false);
            System.out.println(fileName + " " + poziomNatezenia + " " + poraDnia);

            //wyswietl wykresy funkcji fuzyfikacji i defuzyfikacji
            FuzzyRuleSet fuzzyRuleSet = fis.getFuzzyRuleSet();
            fuzzyRuleSet.chart();

            //zadaj wartosci wejsciowe
            fuzzyRuleSet.setVariable("poziom_natezenia", poziomNatezenia);
            fuzzyRuleSet.setVariable("pora_dnia", poraDnia);
            //logika sterownika
            fuzzyRuleSet.evaluate();

            //graficzna prezentacja wyjscia
            fuzzyRuleSet.getVariable("zmiana_natezenia").chartDefuzzifier(true);
            System.out.println(fuzzyRuleSet);

        } catch (ArrayIndexOutOfBoundsException ex) {
            System.out.println("Niepoprawna liczba parametrow. Przyklad: java FuzzyExample string<plik_fcl> int<poziom natezenia> int<pora dnia>");
        } catch (NumberFormatException ex) {
            System.out.println("Niepoprawny parametr. Przyklad: java FuzzyExample string<plik_fcl> int<poziom natezenia> int<pora dnia>");
        } catch (Exception ex) {
            System.out.println(ex.toString());
        }
    }

}