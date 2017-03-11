//package swl.monitor;
package com.android.server;

/**
 * 
 * @author Benjamin
 * This is not a public class, only visible in package swl.monitor
 */

class PolicyManager {

	private static PolicyManager sInstance;

    	static {
        	/*
         	* Load the library.  If it's already loaded, this does nothing.
         	*/
System.err.println("daiting policy manager before load ");
        	//System.loadLibrary("android_policy"); //if kernel we may need this step, otherwise we manually load the LKM
System.err.println("daiting policy manager loading call ");//caller info
    	}

	public static synchronized PolicyManager getDefault() {
System.err.println("daiting policy manager in ");
		if (sInstance == null) {
System.err.println("daiting policy manager instance null ");
			sInstance = new PolicyManager();
		}
System.err.println("daiting policy manager instance ");
		return sInstance;
	}

	public int PolicyOpen(){
System.err.println("daiting policy manager open ");
		return native_PolicyOpen();
	}
	
	//clear all rules
	public void PolicyClearRules(int desc){
		native_PolicyClearRules(desc);
	}

	public void PolicyAddRule(int desc, String r){
System.err.println("daiting policy manager add ");
		native_PolicyAddRule(desc, r);
System.err.println("daiting policy manager after add ");
	}	

	public int PolicyGetRuleNum(int desc){
		return native_PolicyGetRuleNum(desc);
	}
	
	public void PolicyExternalStorage(int desc, String r) {
		native_PolicyExternalStorage(desc, r);	
	}

	/*
	//get the index-th rule
	public String PolicyGetNthRule(int desc, int index){
		return native_PolicyGetNthRule(desc, index);
	}
	*/	

	public void PolicyClose(int desc){
		native_PolicyClose(desc);
	}

	/*
	//dump all the rules
	public void PolicyDump(int desc){
		System.out.println(PolicyGetRuleNum(desc));
		for(int i = 0; i < PolicyGetRuleNum(desc); i++)
		{
			String s = PolicyGetNthRule(desc,i);
			if(s!=null)
				System.out.println(s);
		}
	}
	*/
	private native int native_PolicyOpen();
	private native void native_PolicyClearRules(int desc);
	private native void native_PolicyAddRule(int desc, String r);
	private native int native_PolicyGetRuleNum(int desc);
	private native void native_PolicyExternalStorage(int desc, String r);
	//private native String native_PolicyGetNthRule(int desc, int index);
	private native void native_PolicyClose(int desc);
}
