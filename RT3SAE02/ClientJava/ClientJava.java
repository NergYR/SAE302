

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
        
    }

    public void RunSSH(){
        
    }
}
