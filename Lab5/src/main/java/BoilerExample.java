import net.sourceforge.jFuzzyLogic.FIS;
import net.sourceforge.jFuzzyLogic.rule.FuzzyRuleSet;

public class BoilerExample {
    public static void main(String argv[]) {
        String fileName = "fuzzy_volume_boiler.fcl";
        int []tempKotla = {30, 50, 70};
        int []poraRoku = {0, 7};
        int []poraDnia = {0, 12};
        FIS fis = FIS.load(fileName,false);

        //wyswietl wykresy funkcji fuzyfikacji i defuzyfikacji

        FuzzyRuleSet fuzzyRuleSet = fis.getFuzzyRuleSet();
        fuzzyRuleSet.setVariable("temperatura_kotla", 45);
        fuzzyRuleSet.setVariable("pora_dnia", 0);
        fuzzyRuleSet.setVariable("pora_roku", 0);
        fuzzyRuleSet.evaluate();
        fuzzyRuleSet.getVariable("zmiana_temperatury_kotla").chartDefuzzifier(true);
        fuzzyRuleSet.chart();


        test(fuzzyRuleSet, tempKotla, poraRoku, poraDnia);
    }

    static public void test(FuzzyRuleSet fuzzyRuleSet, int []tempKotla, int []poraRoku, int []poraDnia) {
        for (int temp = 0; temp < tempKotla.length; temp++) {
            for (int rok = 0; rok < poraRoku.length; rok++) {
                for (int dzien = 0; dzien < poraDnia.length; dzien++) {
                    fuzzyRuleSet.setVariable("temperatura_kotla", tempKotla[temp]);
                    fuzzyRuleSet.setVariable("pora_dnia", poraDnia[dzien]);
                    fuzzyRuleSet.setVariable("pora_roku", poraRoku[rok]);
                    fuzzyRuleSet.evaluate();
                    System.out.println(tempKotla[temp] + ":" + poraDnia[dzien] + ":" + poraRoku[rok] + "=>" +
                            fuzzyRuleSet.getVariable("zmiana_temperatury_kotla").getLatestDefuzzifiedValue());
                }
            }
        }

    }
}
