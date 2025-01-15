import java.util.*;

public class Graphics {
    
    public void Console_DisplayFile(List<List<String>> file){
        if(file != null && file.size()>0){
            List<String> col = new ArrayList<>();
            for(int i=0;i<file.size();i++){
                for(int j=0;j<file.get(i).size();j++){
                    System.out.print(file.get(i).get(j)+"|");
                }
            }
        }
    }
}
