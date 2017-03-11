package swl.monitor;


/**
 * 
 * @author xiaolei
 * This is not a public class, only visible in package swl.monitor
 */

class SecEnvManager {

	private static SecEnvManager sInstance;

    	static {
        	/*
         	* Load the library.  If it's already loaded, this does nothing.
         	*/
        	System.loadLibrary("android_secenv");
    	}

	public static synchronized SecEnvManager getDefault() {
		if (sInstance == null) {
			sInstance = new SecEnvManager();
		}
		return sInstance;
	}

	public int SecEnvOpen(){
		return native_SecEnvOpen();
	}
	
	//clear all rules
	public void SecEnvClearRules(int desc){
		native_SecEnvClearRules(desc);
	}

	public void SecEnvAddRule(int desc, String r){
		native_SecEnvAddRule(desc, r);
	}	

	public int SecEnvGetRuleNum(int desc){
		return native_SecEnvGetRuleNum(desc);
	}

	//get the index-th rule
	public String SecEnvGetNthRule(int desc, int index){
		return native_SecEnvGetNthRule(desc, index);
	}	

	public void SecEnvClose(int desc){
		native_SecEnvClose(desc);
	}

	//dump all the rules
	public void SecEnvDump(int desc){
		System.out.println(SecEnvGetRuleNum(desc));
		for(int i = 0; i < SecEnvGetRuleNum(desc); i++)
		{
			String s = SecEnvGetNthRule(desc,i);
			if(s!=null)
				System.out.println(s);
		}
	}

	private native int native_SecEnvOpen();
	private native void native_SecEnvClearRules(int desc);
	private native void native_SecEnvAddRule(int desc, String r);
	private native int native_SecEnvGetRuleNum(int desc);
	private native String native_SecEnvGetNthRule(int desc, int index);
	private native void native_SecEnvClose(int desc);
}
