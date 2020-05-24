public class Getter {
    public static void main(String args[]) throws Exception {
        AnimeDescriptionGetter getter = new AnimeDescriptionGetter();
        for (int i = 5000; i < 11800; i++) {
            getter.getDescription("http://www.repozytorium.fn.org.pl/?q=pl/node/" + i + "/");
        }
        getter.close();
    }
}
