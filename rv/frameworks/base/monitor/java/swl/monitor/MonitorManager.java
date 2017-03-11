package swl.monitor;

import android.app.Activity;

import android.util.Log;
import android.content.ContextWrapper;
import android.content.Context;
import android.app.Application;
import android.os.Process;

import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.*;

import java.util.Vector;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

import android.net.Uri;
import android.provider.ContactsContract;

import android.os.ServiceManager;
import android.os.IBinder;
import com.swl.internal.monitor.IMonitorManager;
import com.swl.internal.monitor.IMonitorManagerCallback;
import android.os.RemoteException;
import android.provider.Browser;
import android.provider.MonitorConstants;
import android.provider.CallLog;
import android.provider.Telephony;
import android.provider.Calendar;
/**
 * 
 * @author xiaolei
 *
 */

public class MonitorManager extends IMonitorManagerCallback.Stub{
	private static MonitorManager mInstance;
    	private static IMonitorManager sMonitorManager;

	static public final int ZONE_POLICY = MonitorConstants.ZONE_POLICY;
	static public final int PACKAGE_POLICY = MonitorConstants.PACKAGE_POLICY;

	static public final int ACCOUNT_POLICY = MonitorConstants.ACCOUNT_POLICY;
	static public final int CALENDAR_POLICY = MonitorConstants.CALENDAR_POLICY;
	static public final int CAMERA_POLICY = MonitorConstants.CAMERA_POLICY;
	static public final int CONTACT_POLICY = MonitorConstants.CONTACT_POLICY;
	static public final int HISTORY_BOOKMARK_POLICY = MonitorConstants.HISTORY_BOOKMARK_POLICY;
	static public final int LOCATION_POLICY = MonitorConstants.LOCATION_POLICY;
	static public final int LOG_POLICY = MonitorConstants.LOG_POLICY;
	static public final int MICROPHONE_POLICY = MonitorConstants.MICROPHONE_POLICY;
	static public final int PHONE_STATE_POLICY = MonitorConstants.PHONE_STATE_POLICY;
	static public final int SMS_POLICY = MonitorConstants.SMS_POLICY;
	static public final int SUBSCRIBED_FEED_POLICY = MonitorConstants.SUBSCRIBED_FEED_POLICY;
	static public final int SDCARD_POLICY = MonitorConstants.SDCARD_POLICY;
	static public final int INTERNET_POLICY = MonitorConstants.INTERNET_POLICY;
	static public final int ADMIN_POLICY = MonitorConstants.ADMIN_POLICY;
	
	static public final String[] permission = {"Account", "Calendar", "Camera", "Contacts", "History Bookmarks", "Location", 
							"Logs", "Microphone", "Phone State", "SMS", "Subscribed Feeds", "SDcard", "Internet", "Admin"};
	
	String TAG = "MonitorManager";
	boolean DEBUG = true;
	//config enable
	/*
	private boolean enable = false;
	private String mode = "UnZone";
	*/	

	/*
	 * config_**_synchronized indicates whether config is synchronized or not.
	 * currently obsolete;
	 */
	private boolean config_enable_synchronized = false;
	private boolean config_mode_synchronized = false;
	

	//synchronize config "enable", can not launch content provider before system ready
	/*
	private int synchronizeEnableConfig()
	{
		if(!config_enable_synchronized)
		{
			//set config_synchronized as true first to avoid recursive loop
			config_enable_synchronized = true;
			try{
				enable = getEnableConfig();
			}catch(Exception e){
				//synchronization failure
				config_enable_synchronized = false;
			}
		}
		return 0;
	}

	//synchronize config "mode", catch the failure because it can not launch content provider before system ready
	private int synchronizeModeConfig()
	{
		if(!config_enable_synchronized)
		{
			//set config_synchronized as true first to avoid recursive loop
			config_mode_synchronized = true;
			try{
				mode = getModeConfig();	
			}catch(Exception e){
				//synchronization failure
				config_mode_synchronized = false;
			}
		}
		return 0;
	}
	*/
	/*
	 * provide a public interface to resynchronize config, set related synchronized value as false
	 * to reduce overhead of updating at once in MonitorManagerService
 	 * currently obsolete;
	 */
	
	public int resynchronizeConfig(String item)
	{
		if(item.equals(MonitorConstants.CONFIG_ENABLE))
			config_enable_synchronized = false;
		if(item.equals(MonitorConstants.CONFIG_MODE))	
			config_mode_synchronized = false;
		return 0;
	}
	

    	private MonitorManager() {
        	//nothing
    	}

	public Vector Args(Object... args) {
		Vector v = new Vector();
		for(int i = 0; i < args.length; i++)
			v.add(args[i]);
		return v;
	}

	//order: account, calendar, camera, contacts, history bookmarks, location, logs, microphone, phone state, sms, subscribed feeds
	public String generatePermission(boolean... args){
		String perm = "";
		for(int i = 0; i < args.length; i++)
		{
			if(args[i])
				perm += "1";
			else
				perm += "0";
		}
		//if args is not long enough, pad "1" at the end
		if(args.length < permission.length)
		{
			for(int i=0; i < permission.length - args.length; i++)
				perm += "1";
		}
		return perm;
	}

	public int getCurrentPid(){
		return Process.myPid();
	}

	public String getPackageNameForMonitoredProcess(int MonitoredProcessId){
		String packagename = "UNKNOWN";
		/*
		if (getBaseContext() != null) {
			packagename += getBaseContext().getPackageName();
		}*/

		/*if (getApplicationContext() != null) {

			packagename += getApplicationContext().toString();
		}*/
		/*
		if (getApplicationInfo() != null) {
			packagename += "WTF";
		}*/
		/*
		if (getCacheDir() != null) {
			packagename += "Cache";
		}*/
		/*
		 * if(getExternalCacheDir()!=null) { packagename += "EEE"; }
		 * if(getExternalFilesDir()!=null) { packagename += "FFF"; }
		 */
		/*
		if (getPackageCodePath() != null) {
			packagename += "CCC";
		}
		if (getPackageResourcePath() != null) {
			packagename += "PPP";
		}*/
		int pid = MonitoredProcessId;
		//int uid = Process.myUid();
		//Log.i("pid", ""+pid);
		
		try{
		//FileReader fr = new FileReader("/proc/"+pid+"/cmdline");
		File file = new File("/proc/"+pid+"/cmdline");
		FileInputStream fin = new FileInputStream(file);
		//BufferedReader br = new BufferedReader(fr);
		int ch;
		StringBuffer strContent = new StringBuffer("");
		while((ch=fin.read())!=-1)
			strContent.append((char)ch);
		//FileInputStream fstream = new FileInputStream("/proc"+pid+"/cmdline");
      		//DataInputStream in = new DataInputStream(fstream);
      		//BufferedReader br = new BufferedReader(new InputStreamReader(in));
		String s = strContent.toString();
		String[] separated = s.split(Character.toString((char)0x00));
		
		/*
		while((s = br.readLine()) != null) {
			packagename += s;
		}*/
		//s = br.readLine();
		//br.close();
		fin.close();
		packagename = separated[0];
		for(int i=1;i<separated.length;i++)
			packagename += "|"+separated[i];
		//Log.i("br", separated[0]);
		
		//fr.close(); 
		}catch(IOException e)
		{
		}

		//cmdline may not be exact with packagename, so we need match package name
		//immediately pass system_server to avoid dead loop
		if(packagename.equals("system_server"))
			return packagename;
		Map res = getAllPackageZone();
		if(res.size()>0)
		{
			Iterator iterator = res.entrySet().iterator();
			while(iterator.hasNext()){
				Map.Entry m = (Map.Entry)iterator.next();
				String t = (String)m.getKey();
				if(packagename!=null && packagename.startsWith(t))
				{
					packagename = t;
					break;
				}
			}
		}
		return packagename;
	}

	//Package 'pname' is invoking API 'API' with Arguments 'args'
	public int Hook(String pname, String API, Vector args) {
		
		/*
		 * immediately allow system_server to access, to avoid reach getEnableConfig(eg. which 
		 * needs access content provider currently) before content provider ready
             	 */
		if(pname.equals("system_server") || pname.equals("zygote"))
			return 0;

		//to avoid dead loop caused by getEnableConfig, we directly return 0 when accessing monitor policy
		if(API.equals("ContentResolver.query"))
		{
			Uri uri = (Uri)args.get(0);
			if(uri.getAuthority().equals(MonitorConstants.AUTHORITY))
			{
				Log.i(TAG, "Monitor Manager is accessing policies");
				return 0;
			}
		}
		//if disabled
		if(!getEnableConfig())
		{
			return 0;
		}

		String packagename = pname;

		//packagename += Integer.toString(pid);
		//packagename += " "+uid;
		if(DEBUG)
		Log.i(TAG,packagename + " is invoking "+ API);
		/*if(checkPolicy(packagename)!=0)
		{
			Log.i(TAG, API+" invocation is blocked by MonitorManager");
			return -1;
		}else
		{
			Log.i(TAG, API+" invocation is approved by MonitorManager");
		}*/

		if(API.equals("ContentResolver.query"))
		{
			Uri uri = (Uri)args.get(0);
			Log.d(TAG, "Authority of caller " + uri.getPath());
			//Log.d(TAG, "Authority of Contact " + ContactsContract.AUTHORITY_URI.getAuthority());
			//read Contact
			if(uri.getAuthority().equals(ContactsContract.AUTHORITY_URI.getAuthority()))
			{
				//Log.e(TAG, "path " + uri.getAuthority());
				//Log.e(TAG, "path2" + ContactsContract.AUTHORITY_URI.getAuthority());
				boolean adchoice = getPolicy(CONTACT_POLICY, packagename);
				//Log.d(TAG, "policy " + adchoice);
				if(!adchoice)
				{
					if(DEBUG)
					Log.i(TAG, API+" invocation is blocked by MonitorManager");
					return -1;
				}
			}else if(uri.getAuthority().equals(Browser.BOOKMARKS_URI.getAuthority()))
			{
				boolean adchoice = getPolicy(HISTORY_BOOKMARK_POLICY, packagename);
				//Log.d(TAG, "policy " + adchoice);
				if(!adchoice)
				{
					if(DEBUG)
					Log.i(TAG, API+" invocation is blocked by MonitorManager");
					return -1;
				}
			}else if(uri.getAuthority().equals(CallLog.AUTHORITY))
			{
				boolean adchoice = getPolicy(LOG_POLICY, packagename);
				//Log.d(TAG, "policy " + adchoice);
				if(!adchoice)
				{
					if(DEBUG)
					Log.i(TAG, API+" invocation is blocked by MonitorManager");
					return -1;
				}
			}else if(uri.getAuthority().equals(Telephony.Sms.CONTENT_URI.getAuthority()) ||
				uri.getAuthority().equals(Telephony.Mms.CONTENT_URI.getAuthority()) ||
				uri.getAuthority().equals(Telephony.MmsSms.CONTENT_URI.getAuthority()))
			{
				boolean adchoice = getPolicy(SMS_POLICY, packagename);
				//Log.d(TAG, "policy " + adchoice);
				if(!adchoice)
				{
					if(DEBUG)
					Log.i(TAG, API+" invocation is blocked by MonitorManager");
					return -1;
				}
			}else if(uri.getAuthority().equals(Calendar.AUTHORITY))
			{
				boolean adchoice = getPolicy(CALENDAR_POLICY, packagename);
				//Log.d(TAG, "policy " + adchoice);
				if(!adchoice)
				{
					if(DEBUG)
					Log.i(TAG, API+" invocation is blocked by MonitorManager");
					return -1;
				}
			}else if(uri.getAuthority().equals("subscribedfeeds"))
			{
				boolean adchoice = getPolicy(SUBSCRIBED_FEED_POLICY, packagename);
				//Log.d(TAG, "policy " + adchoice);
				if(!adchoice)
				{
					if(DEBUG)
					Log.i(TAG, API+" invocation is blocked by MonitorManager");
					return -1;
				}
			}

		}else if(API.startsWith("LocationManagerService"))
		{
			boolean adchoice = getPolicy(LOCATION_POLICY, packagename);
			if(!adchoice)
			{
				if(DEBUG)
				Log.i(TAG, API+" invocation is blocked by MonitorManager");
				return -1;
			}
		}else if(API.startsWith("PhoneSubInfoProxy"))
		{
			boolean adchoice = getPolicy(PHONE_STATE_POLICY, packagename);
			if(!adchoice)
			{
				if(DEBUG)
				Log.i(TAG, API+" invocation is blocked by MonitorManager");
				return -1;
			}
		}else if(API.startsWith("Camera"))
		{
			boolean adchoice = getPolicy(CAMERA_POLICY, packagename);
			if(!adchoice)
			{
				if(DEBUG)
				Log.i(TAG, API+" invocation is blocked by MonitorManager");
				return -1;
			}			
		}else if(API.startsWith("MediaRecorder"))
		{
			boolean adchoice = getPolicy(MICROPHONE_POLICY, packagename);
			if(!adchoice)
			{
				if(DEBUG)
				Log.i(TAG, API+" invocation is blocked by MonitorManager");
				return -1;
			}			
		}else if(API.startsWith("AccountManagerService"))
		{
			boolean adchoice = getPolicy(ACCOUNT_POLICY, packagename);
			if(!adchoice)
			{
				if(DEBUG)
				Log.i(TAG, API+" invocation is blocked by MonitorManager");
				return -1;
			}			
		}
		if(DEBUG)
		Log.i(TAG, API+" invocation is approved by MonitorManager");
		return 0;

	}

	//set or create a zone, if permission is null, remove zone 'zname'
	public int setZone(String zname, String permission)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null && sMonitorManager.setZone(zname, permission)==0)
				return 0;
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}

	//return all (zone, permission) pairs
	public Map getAllZone()
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getAllZone();
			else
				return null;
		} catch(RemoteException e){
			e.printStackTrace();
			return null;
		}
	}

	//return permission of one zone
	public String getPermissionFromZone(String zname)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getPermissionFromZone(zname);
			else
				return null;
		} catch(RemoteException e){
			e.printStackTrace();
			return null;
		}
	}

	//return zone of one package
	public String getZoneFromPackage(String pname)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getZoneFromPackage(pname);
			else
				return null;
		} catch(RemoteException e){
			e.printStackTrace();
			return null;
		}
	}

	//return all (package, zone) pairs
	public Map getAllPackageZone()
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getAllPackageZone();
			else
				return null;
		} catch(RemoteException e){
			e.printStackTrace();
			return null;
		}
	}

	//return all package included in one zone
	public String[] getPackageFromZone(String zname)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getPackageFromZone(zname);
			else
				return null;
		} catch(RemoteException e){
			e.printStackTrace();
			return null;
		}
	}

	//set the (package,zone) pair, if zname is null, then remove the package from any zone
	public int setPackageZone(String pname, String zname)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null && sMonitorManager.setPackageZone(pname, zname)==0)
				return 0;
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}

	private int setPolicy(int POLICY_CATEGORY, String packagename, boolean policy) {

		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null && sMonitorManager.setPolicy(POLICY_CATEGORY, packagename, policy)==0)
				return 0;
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}
	


	private boolean getPolicy(int POLICY_CATEGORY, String packagename) {

		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getPolicy(POLICY_CATEGORY, packagename);
			else
				return true;
		} catch(RemoteException e){
			e.printStackTrace();
			return true;
		}
	}

	private int checkPolicy(String packagename) {
		
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null && sMonitorManager.checkPolicy(packagename)==0)
				return 0;
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}

	//get config item
	private String getConfigItem(String item)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
				return sMonitorManager.getConfigItem(item);
		} catch(RemoteException e){
			e.printStackTrace();
			return null;
		}
		return null;
	}

	//set config item
	private int setConfigItem(String item, String value)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null && sMonitorManager.setConfigItem(item, value)==0)
				return 0;
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}
	
	//get config enable, default false
	public boolean getEnableConfig()
	{
		String ret = getConfigItem(MonitorConstants.CONFIG_ENABLE);
		if(ret!= null && ret.equals("true"))
			return true;
		else
			return false;
	}
	
	//set config enable
	public int setEnableConfig(boolean en)
	{
		/* don't need update config "enable" manually,
		 * config "enable" will be updated by callback from MonitorManagerService automatically
		 */
		int ret = -1;
		if(en)
		{
			ret = setConfigItem(MonitorConstants.CONFIG_ENABLE, "true");
		}
		else
		{
			ret = setConfigItem(MonitorConstants.CONFIG_ENABLE, "false");
		}

		return ret;
	}
	
	//get config mode, default UnZone
	public String getModeConfig()
	{
		//default "UnZone"
		String ret = getConfigItem(MonitorConstants.CONFIG_MODE);
		if(ret == null)
			ret = "UnZone";
		return ret;	
	}

	//set config mode
	public int setModeConfig(String mode)
	{	
		return setConfigItem(MonitorConstants.CONFIG_MODE, mode);
	}

	//register into MonitorManagerService
	static private int registerCallback(IMonitorManagerCallback mm)
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null && sMonitorManager.registerCallback(mm)==0)
				return 0;
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}	

	//@ policy by benjamin
	public int updateKernel()
	{
		try {
			IBinder b = ServiceManager.getService("monitor");
        		sMonitorManager = IMonitorManager.Stub.asInterface(b);
			if(sMonitorManager !=null)
			{
				sMonitorManager.updateKernel();
				return 0;
			}	
			else
				return -1;
		} catch(RemoteException e){
			e.printStackTrace();
			return -1;
		}
	}
	//# end by benjamin
	
	public static synchronized MonitorManager getDefault() {
		if (mInstance == null) {
			mInstance = new MonitorManager();
			/* currently obsolete: register mInstance to MonitorManagerService;
			 * different process have different instance, so "enable" config will not take effect by simply
  			 * modifying one of them. So we need register all instances into service, and let service manage 
 			 * all of them.
			 */
			//registerCallback(mInstance);
		}
		return mInstance;
	}

}
