import java.util.concurrent.Executors;

public class Getter {
    public static void main(String args[]) throws Exception {
        AnimeDescriptionGetter getter = new AnimeDescriptionGetter();

        Executors.newCachedThreadPool().submit(() -> {
            try {
                for (int i = 5000; i < 11800; i++) {
                    if (getter.getDocumentary("http://www.repozytorium.fn.org.pl/?q=pl/node/" + i + "/") > 1100) break;
                }
                System.out.println("Documentary is done!");
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
        try {
            for (int i = 1; i < 15000; i++) {
                if (getter.getAnime("https://shinden.pl/series/" + i) > 1100) break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println("Anime is done!");
        getter.close();
    }
}
