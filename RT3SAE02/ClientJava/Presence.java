import java.util.*;

public class Presence {
    
    private List<List<String>> list_student = null;

    public Presence(){
        this.list_student = new ArrayList<>();
    }

    public void Console_MarkPresenceStudent(List<Integer> pres){
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
    }
}
