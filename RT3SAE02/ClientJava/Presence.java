import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Presence {
    
    private List<List<String>> list_student = null;

    public Presence(List<List<String>> l){
        this.list_student = l;
    }

    public List<List<String>> getList(){
        return this.list_student;
    }

    public void MarkPresence(List<Integer> pres){
        list_student.get(0).add("présence");
        for (int i = 1; i < this.list_student.size(); i++) {
            if(i<pres.size()){
                if(pres.get(i)==1){
                    list_student.get(i).add("présent");
                }
                else if(pres.get(i)==0){
                    list_student.get(i).add("absent");
                }
                else{
                    list_student.get(i).add("NaN");
                }
            }
            else{
                list_student.get(i).add("NaN");
            }
        }
        System.out.println(this.list_student);
    }

    public List<Integer> Appel(){
        List<Integer> presence = new ArrayList<>();
        Scanner s = new Scanner(System.in);
        for(int i=1;i<this.list_student.size();i++){
            System.out.println(this.list_student.get(i));
            System.out.print("Si l'étudiant est présent entrez présent (respectivement absent) :");
            String rep = s.nextLine();
            if("présent".equals(rep)){
                presence.add(1);
            }
            else{
                presence.add(0);
            }
        }
        return presence;
    }
}
