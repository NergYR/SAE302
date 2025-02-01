
import java.util.*;

public class ClientJava {

    private String cert = "";
    private String addr = "";

    public ClientJava(String cert, String addr){
        this.cert = cert;
        this.addr = addr;
    }

    public ClientJava(String addr){
        this.addr = addr;
    }

    public void Start(){
        if(cert != ""){
            RunSSH();
        }
        else{
            Run();
        }
    }

    public void Run(){
        CommServ cs = new CommServ(this.addr);
        List<String> name = cs.GetInfo();
        System.out.println(name);
        cs.SendInfo("RT1FA.csv\n");
        List<List<String>> file = cs.GetFile();
        Presence p = new Presence(file);
        p.Console_MarkPresenceStudent(p.Appel());
        System.out.println(" 6");
        cs.SendFile(p.getList());
        cs.Disconnect();
    }

    public void RunSSH(){
        
    }
}
