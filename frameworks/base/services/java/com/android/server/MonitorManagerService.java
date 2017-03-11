//package swl.monitor;
package com.android.server;

import android.content.Context;
import android.os.RemoteException;
import android.os.IBinder;
import android.os.Environment;

import android.net.Uri;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.database.Cursor;
import android.util.Log;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;
import java.util.Iterator;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import android.monitor.IMonitorManager;
//import com.swl.internal.monitor.IMonitorManagerCallback;
import android.provider.MonitorConstants;


//import swl.monitor.MonitorManager;
//import swl.monitor.SecEnvManager;
//@ policy by benjamin
//import com.android.server.PolicyManager;
//import PolicyManager;
import android.os.Binder;
import android.os.Process;
//# end by benjamin

/**
 * 
 * @author xiaolei
 * @description all apps can read policy, only apps with SignatureOrSystem permission "android.permission.WRITE_MONITOR" can write policy
 */

public class MonitorManagerService extends IMonitorManager.Stub {

	
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

	private static final String[] permission = {"Account", "Calendar", "Camera", "Contacts", "History Bookmarks", "Location", 
							"Logs", "Microphone", "Phone State", "SMS", "Subscribed Feeds", "SDcard", "Internet", "Admin"};

	private String TAG = "MonitorManagerService";
	private String DEFAULT_ZONE = "UnZone";

	public String mzname = "notset";
	public String mpermission = "notset";


	/*
	 * maintain all MonitorManager instances for updating config of each instance
	 * currently obsolete; reason: system will hang up when making to call back to STOPPED process
	 */ 
	private Vector mmVector; 

	private final Context mContext;

	public MonitorManagerService(Context context) {
		mContext = context;
		mmVector = new Vector();
    	}

	//currently obsolete;
	/*public int registerCallback(IMonitorManagerCallback mm) {
		mmVector.add(mm);
		return 0;
	}*/
	


	public int setZone(String zname, String permission) {
		//make sure only apps with SignatureOrSystem permission "android.permission.WRITE_MONITOR" can write policy
		/*final int uid = Binder.getCallingUid();
		if(uid != Process.SYSTEM_UID && uid != 0 )
			mContext.enforceCallingPermission("android.permission.WRITE_MONITOR","Writing monitor policy");

		//remove this zone
		if(permission == null)
		{
			ContentResolver crInstance = mContext.getContentResolver();
			//remove (zone,permission) pair from zone_permission table
			crInstance.delete(MonitorConstants.ZONE.CONTENT_URI, "zname = ?", new String[]{zname});
			//remove all (package, zone) pair from package_zone table
			String[] packages = getPackageFromZone(zname);
			for(int i = 0; i < packages.length; i++)
				//this function will autoremove policies of the package in all policy table
				setPackageZone(packages[i],null);	
			
		}else
		{
        		ContentResolver crInstance = mContext.getContentResolver();
        		ContentValues newTaskValue = new ContentValues();
			newTaskValue.put("zname", zname);
			newTaskValue.put("perm", permission);
             		Log.i(TAG, "set database");
			Cursor c = crInstance.query(MonitorConstants.ZONE.CONTENT_URI,null, "zname = ?", new String[]{zname},null);
			if(c==null || c.getCount()==0)//doesn't exist
				crInstance.insert(MonitorConstants.ZONE.CONTENT_URI, newTaskValue);
			else//already exist
				crInstance.update(MonitorConstants.ZONE.CONTENT_URI,newTaskValue, "zname = ?", new String[]{zname});
			c.close();
			//reset all (package, zone) pair from package_zone table
			String[] packages = getPackageFromZone(zname);
			for(int i = 0; i < packages.length; i++) 
				//this function will autoreset policies of the package in all policy table
				setPackageZone(packages[i],zname);		
			
			
		}*/

		mzname = zname;
		mpermission = permission;
System.out.println("daiting setZone server: "+zname+" "+permission);//caller info
		//end by benjamin
		updateKernel();
		//end by benjamin

		return 0;
	}

	public Map getAllZone()
	{
		HashMap<String,String> res = new HashMap<String,String>();
		String zname;
		String permission;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.ZONE.CONTENT_URI,null, null, null,null);
		if(c==null)
			return res;
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			do {
				zname = c.getString(0);
				permission = c.getString(1);
				res.put(zname, permission);
			}while(c.moveToNext());
		}
		c.close();
		return res;
	}

	public String getPermissionFromZone(String zname)
	{
		String permission = null;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.ZONE.CONTENT_URI,null, "zname = ?", new String[]{zname},null);
		if(c==null)
			return permission;
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			permission = c.getString(1);
		}
		c.close();
		return permission;
	}

	public String getZoneFromPackage(String pname)
	{
		String zname = null;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.PACKAGE.CONTENT_URI,null, "pname = ?", new String[]{pname},null);
		if(c==null)
			return zname;
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			zname = c.getString(1);
		}
		c.close();
		return zname;
	}

	public Map getAllPackageZone()
	{
		HashMap<String,String> res = new HashMap<String,String>();
		String pname;
		String zname;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.PACKAGE.CONTENT_URI,null, null, null,null);
		if(c==null)
			return res;
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			do {
				pname = c.getString(0);
				zname = c.getString(1);
				res.put(pname, zname);
			}while(c.moveToNext());
		}
		c.close();
		return res;
	}

	public String[] getPackageFromZone(String zname)
	{
		String[] packages = {};
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.PACKAGE.CONTENT_URI,null, "zname = ?", new String[]{zname},null);
		if(c==null)
			return packages;
		if(c.getCount()!=0)
		{
			packages = new String[c.getCount()];
			c.moveToFirst();
			int i=0;
			do
			{
				packages[i++] = c.getString(0);
			}while(c.moveToNext());
		}
		c.close();
		return packages;
	}

	public int setPackageZone(String pname, String zname)
	{
		//make sure only apps with SignatureOrSystem permission "android.permission.WRITE_MONITOR" can write policy
		final int uid = Binder.getCallingUid();
		if(uid != Process.SYSTEM_UID && uid != 0 )
			mContext.enforceCallingPermission("android.permission.WRITE_MONITOR","Writing monitor policy");
		//delete 
		if(zname == null)
		{
			ContentResolver crInstance = mContext.getContentResolver();
			//remove (package,zone) pair from package_zone table
			crInstance.delete(MonitorConstants.PACKAGE.CONTENT_URI, "pname = ?", new String[]{pname});
			//remove policies of the package in all policy table
			crInstance.delete(MonitorConstants.ACCOUNT.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.CALENDAR.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.CAMERA.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.CONTACT.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.HISTORY_BOOKMARK.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.LOCATION.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.LOG.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.MICROPHONE.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.PHONE_STATE.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.SMS.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.SUBSCRIBED_FEED.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.SDCARD.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.INTERNET.CONTENT_URI, "pname = ?", new String[]{pname});
			crInstance.delete(MonitorConstants.ADMIN.CONTENT_URI, "pname = ?", new String[]{pname});

			return 0;
		}else
		{
			Log.i(TAG, "setPackageZone");

        		ContentResolver crInstance = mContext.getContentResolver();
        		ContentValues newTaskValue = new ContentValues();
			newTaskValue.put("pname", pname);
			newTaskValue.put("zname", zname);

			String permission = getPermissionFromZone(zname);
			Cursor c = crInstance.query(MonitorConstants.PACKAGE.CONTENT_URI, null, "pname = ?", new String[]{pname},null);
			//update package_zone table
			if(c==null || c.getCount()==0)//doesn't exist
			{
				crInstance.insert(MonitorConstants.PACKAGE.CONTENT_URI, newTaskValue);
			}
			else//already exist
				crInstance.update(MonitorConstants.PACKAGE.CONTENT_URI,newTaskValue, "pname = ?", new String[]{pname});
			c.close();
			//update all policy tables
/*

			if(permission.charAt(0)=='0')
				setPolicy(ACCOUNT_POLICY, pname, false);
			else
				setPolicy(ACCOUNT_POLICY, pname, true);
			if(permission.charAt(1)=='0')
				setPolicy(CALENDAR_POLICY, pname, false);
			else
				setPolicy(CALENDAR_POLICY, pname, true);
			if(permission.charAt(2)=='0')
				setPolicy(CAMERA_POLICY, pname, false);
			else
				setPolicy(CAMERA_POLICY, pname, true);
			if(permission.charAt(3)=='0')
				setPolicy(CONTACT_POLICY, pname, false);
			else
				setPolicy(CONTACT_POLICY, pname, true);
			if(permission.charAt(4)=='0')
				setPolicy(HISTORY_BOOKMARK_POLICY, pname, false);
			else
				setPolicy(HISTORY_BOOKMARK_POLICY, pname, true);
			if(permission.charAt(5)=='0')
				setPolicy(LOCATION_POLICY, pname, false);
			else
				setPolicy(LOCATION_POLICY, pname, true);
			if(permission.charAt(6)=='0')
				setPolicy(LOG_POLICY, pname, false);
			else
				setPolicy(LOG_POLICY, pname, true);
			if(permission.charAt(7)=='0')
				setPolicy(MICROPHONE_POLICY, pname, false);
			else
				setPolicy(MICROPHONE_POLICY, pname, true);
			if(permission.charAt(8)=='0')
				setPolicy(PHONE_STATE_POLICY, pname, false);
			else
				setPolicy(PHONE_STATE_POLICY, pname, true);
			if(permission.charAt(9)=='0')
				setPolicy(SMS_POLICY, pname, false);
			else
				setPolicy(SMS_POLICY, pname, true);
			if(permission.charAt(10)=='0')
				setPolicy(SUBSCRIBED_FEED_POLICY, pname, false);
			else
				setPolicy(SUBSCRIBED_FEED_POLICY, pname, true);
			if(permission.charAt(11)=='0')
				setPolicy(SDCARD_POLICY, pname, false);
			else
				setPolicy(SDCARD_POLICY, pname, true);
			if(permission.charAt(12)=='0')
				setPolicy(INTERNET_POLICY, pname, false);
			else
				setPolicy(INTERNET_POLICY, pname, true);

			if(permission.charAt(13)=='0')
				setPolicy(ADMIN_POLICY, pname, false);
			else
				setPolicy(ADMIN_POLICY, pname, true);

*/
			return 0;
		}
	}

	public int checkPolicy(String packagename) throws RemoteException{
		Log.i(TAG, "Checking policy for package "+packagename);
        	ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.SMS.CONTENT_URI, null, "pname = ?", new String[]{packagename},null);
		if(c.getCount()==0)
			return 0;
		c.moveToFirst();
		int result = c.getInt(1);
		c.close();
		if(result!=1)
			return -1;
		return 0;
	}

	public int setPolicy(int POLICY_CATEGORY, String packagename, boolean policy) {
		//make sure only apps with SignatureOrSystem permission "android.permission.WRITE_MONITOR" can write policy
		final int uid = Binder.getCallingUid();
		if(uid != Process.SYSTEM_UID && uid != 0 )
			mContext.enforceCallingPermission("android.permission.WRITE_MONITOR","Writing monitor policy");

		Log.i(TAG, "setPolicy");

		switch(POLICY_CATEGORY)	{
		case ACCOUNT_POLICY:
			setPolicyForIndividual(MonitorConstants.ACCOUNT.CONTENT_URI, packagename, policy);
			break;
		case CALENDAR_POLICY:
			setPolicyForIndividual(MonitorConstants.CALENDAR.CONTENT_URI, packagename, policy);
			break;
		case CAMERA_POLICY:
			setPolicyForIndividual(MonitorConstants.CAMERA.CONTENT_URI, packagename, policy);
			break;
		case CONTACT_POLICY:
			setPolicyForIndividual(MonitorConstants.CONTACT.CONTENT_URI, packagename, policy);
			break;
		case HISTORY_BOOKMARK_POLICY:
			setPolicyForIndividual(MonitorConstants.HISTORY_BOOKMARK.CONTENT_URI, packagename, policy);
			break;
		case LOCATION_POLICY:
			setPolicyForIndividual(MonitorConstants.LOCATION.CONTENT_URI, packagename, policy);
			break;
		case LOG_POLICY:
			setPolicyForIndividual(MonitorConstants.LOG.CONTENT_URI, packagename, policy);
			break;
		case MICROPHONE_POLICY:
			setPolicyForIndividual(MonitorConstants.MICROPHONE.CONTENT_URI, packagename, policy);
			break;
		case PHONE_STATE_POLICY:
			setPolicyForIndividual(MonitorConstants.PHONE_STATE.CONTENT_URI, packagename, policy);
			break;
		case SMS_POLICY:
			setPolicyForIndividual(MonitorConstants.SMS.CONTENT_URI, packagename, policy);
			break;
		case SUBSCRIBED_FEED_POLICY:
			setPolicyForIndividual(MonitorConstants.SUBSCRIBED_FEED.CONTENT_URI, packagename, policy);
			break;
		case SDCARD_POLICY:
			setPolicyForIndividual(MonitorConstants.SDCARD.CONTENT_URI, packagename, policy);
			break;
		case INTERNET_POLICY:
			setPolicyForIndividual(MonitorConstants.INTERNET.CONTENT_URI, packagename, policy);
			break;
		case ADMIN_POLICY:
			setPolicyForIndividual(MonitorConstants.ADMIN.CONTENT_URI, packagename, policy);
			break;
		default:
			break;
		}
		return 0;
	}
	
	//uri points to the table to be updated
	public int setPolicyForIndividual(Uri uri, String packagename, boolean policy)
	{
		//make sure only apps with SignatureOrSystem permission "android.permission.WRITE_MONITOR" can write policy
		final int uid = Binder.getCallingUid();
		if(uid != Process.SYSTEM_UID && uid != 0 )
			mContext.enforceCallingPermission("android.permission.WRITE_MONITOR","Writing monitor policy");
        	
        	ContentResolver crInstance = mContext.getContentResolver();
        	ContentValues newTaskValue = new ContentValues();
		newTaskValue.put("pname", packagename);
		newTaskValue.put("adchoice", policy);
             	Log.i(TAG, "setPolicyForIndividual");
		Cursor c = crInstance.query(uri,null, "pname = ?", new String[]{packagename},null);
		//Log.i(TAG, "" + uri + " " + packagename);
		if(c==null || c.getCount()==0)//doesn't exist
			crInstance.insert(uri, newTaskValue);
		else//already exist
			crInstance.update(uri,newTaskValue, "pname = ?", new String[]{packagename});
		c.close();
		return 0;
	}
/*	
	public int setSMSPolicyForIndividual(String packagename, boolean policy) {
        	ContentResolver crInstance = mContext.getContentResolver();
        	ContentValues newTaskValue = new ContentValues();
		newTaskValue.put("pname", packagename);
		newTaskValue.put("adchoice", policy);
             	Log.i(TAG, "set database");
		Cursor c = crInstance.query(MonitorConstants.SMS.CONTENT_URI,null, "pname = ?", new String[]{packagename},null);
		if(c==null || c.getCount()==0)//doesn't exist
			crInstance.insert(MonitorConstants.SMS.CONTENT_URI, newTaskValue);
		else//already exist
			crInstance.update(MonitorConstants.SMS.CONTENT_URI,newTaskValue, "pname = ?", new String[]{packagename});
		c.close();
		return 0;
	}

	public int setContactPolicyForIndividual(String packagename, boolean policy) {
        	ContentResolver crInstance = mContext.getContentResolver();
        	ContentValues newTaskValue = new ContentValues();
		newTaskValue.put("pname", packagename);
		newTaskValue.put("adchoice", policy);
		Cursor c = crInstance.query(MonitorConstants.CONTACT.CONTENT_URI, null, "pname = ?", new String[]{packagename},null);
		if(c==null || c.getCount()==0)//doesn't exist
			crInstance.insert(MonitorConstants.CONTACT.CONTENT_URI, newTaskValue);
		else//already exist
			crInstance.update(MonitorConstants.CONTACT.CONTENT_URI,newTaskValue,"pname = ?", new String[]{packagename});
		c.close();
		return 0;
	}

	public int setLocationPolicyForIndividual(String packagename, boolean policy) {
        	ContentResolver crInstance = mContext.getContentResolver();
        	ContentValues newTaskValue = new ContentValues();
		newTaskValue.put("pname", packagename);
		newTaskValue.put("adchoice", policy);
		Cursor c = crInstance.query(MonitorConstants.LOCATION.CONTENT_URI, null, "pname = ?", new String[]{packagename},null);
		if(c==null || c.getCount()==0)//doesn't exist
			crInstance.insert(MonitorConstants.LOCATION.CONTENT_URI, newTaskValue);
		else//already exist
			crInstance.update(MonitorConstants.LOCATION.CONTENT_URI,newTaskValue,"pname = ?", new String[]{packagename});
		c.close();
		return 0;
	}
*/
	public boolean getPolicy(int POLICY_CATEGORY, String packagename) {
		//Log.e(TAG,"getPolicy is invoked by process "+getCallingPid());
		//all apps can read policy
		//mContext.enforceCallingPermission("android.permission.READ_MONITOR","Reading monitor policy");
		boolean mark = true;
		switch(POLICY_CATEGORY)	{
		case ACCOUNT_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.ACCOUNT.CONTENT_URI, packagename);
			break;
		case CALENDAR_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.CALENDAR.CONTENT_URI, packagename);
			break;
		case CAMERA_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.CAMERA.CONTENT_URI, packagename);
			break;
		case CONTACT_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.CONTACT.CONTENT_URI, packagename);
			break;
		case HISTORY_BOOKMARK_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.HISTORY_BOOKMARK.CONTENT_URI, packagename);
			break;
		case LOCATION_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.LOCATION.CONTENT_URI, packagename);
			break;
		case LOG_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.LOG.CONTENT_URI, packagename);
			break;
		case MICROPHONE_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.MICROPHONE.CONTENT_URI, packagename);
			break;
		case PHONE_STATE_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.PHONE_STATE.CONTENT_URI, packagename);
			break;
		case SMS_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.SMS.CONTENT_URI, packagename);
			break;
		case SUBSCRIBED_FEED_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.SUBSCRIBED_FEED.CONTENT_URI, packagename);
			break;
		case SDCARD_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.SDCARD.CONTENT_URI, packagename);
			break;
		case INTERNET_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.INTERNET.CONTENT_URI, packagename);
			break;
		case ADMIN_POLICY:
			mark = getPolicyForIndividual(MonitorConstants.ADMIN.CONTENT_URI, packagename);
			break;
		default:
			break;
		}
		return mark;
	}

	//uri points to the table to be read
	public boolean getPolicyForIndividual(Uri uri, String packagename) {
		boolean mark = true;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(uri,null, "pname = ?", new String[]{packagename},null);
		if(c==null)
			return mark;
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			if(c.getInt(1)==0)
				mark = false;
		}
		c.close();
		return mark;
	}
/*
	public boolean getSMSPolicyForIndividual(String packagename) {
		boolean mark = true;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.SMS.CONTENT_URI,null, "pname = ?", new String[]{packagename},null);
		if(c==null)
			return mark;
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			if(c.getInt(1)==0)
				mark = false;
		}
		c.close();
		return mark;
	}

	public boolean getContactPolicyForIndividual(String packagename) {
		boolean mark = true;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.CONTACT.CONTENT_URI,null, "pname = ?", new String[]{packagename},null);
		//default true
		if(c==null)
		{
			//Log.e(TAG, "c is null");
			return mark;
		}
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			if(c.getInt(1)==0)
				mark = false;
		}
		c.close();
		return mark;
	}

	public boolean getLocationPolicyForIndividual(String packagename) {
		boolean mark = true;
		ContentResolver crInstance = mContext.getContentResolver();
       		Cursor c = crInstance.query(MonitorConstants.LOCATION.CONTENT_URI,null, "pname = ?", new String[]{packagename},null);
		//default true
		if(c==null)
		{
			//Log.e(TAG, "c is null");
			return mark;
		}
		if(c.getCount()!=0)
		{
			c.moveToFirst();
			if(c.getInt(1)==0)
				mark = false;
		}
		c.close();
		return mark;
	}
*/

	public String getConfigItem(String item) {
		String ret = null;
		//catch the failure because it can not launch content provider before system ready
		try
		{
			ContentResolver crInstance = mContext.getContentResolver();
       			Cursor c = crInstance.query(MonitorConstants.CONFIG.CONTENT_URI,null, MonitorConstants.CONFIG_ITEM+" = ?", new String[]{item},null);		
			//default true
			if(c==null)
			{
				//Log.e(TAG, "c is null");
				return null;
			}
			if(c.getCount()!=0)
			{
				c.moveToFirst();
				ret = c.getString(1);
			}
			c.close();
		}catch (Exception e) {
			e.printStackTrace();
		}
		return ret;
	}

	public int setConfigItem(String item, String value) {
		//make sure only apps with SignatureOrSystem permission "android.permission.WRITE_MONITOR" can write policy
		final int uid = Binder.getCallingUid();
		if(uid != Process.SYSTEM_UID && uid != 0 )	
			mContext.enforceCallingPermission("android.permission.WRITE_MONITOR","Writing monitor policy");

        	ContentResolver crInstance = mContext.getContentResolver();
        	ContentValues newTaskValue = new ContentValues();
		newTaskValue.put(MonitorConstants.CONFIG_ITEM, item);
		newTaskValue.put(MonitorConstants.CONFIG_VALUE, value);
		Uri uri = MonitorConstants.CONFIG.CONTENT_URI;
             	Log.i(TAG, "setConfigItem");
		Cursor c = crInstance.query(uri,null, MonitorConstants.CONFIG_ITEM+" = ?", new String[]{item},null);
		if(c==null || c.getCount()==0)//doesn't exist
			crInstance.insert(uri, newTaskValue);
		else//already exist
			crInstance.update(uri,newTaskValue, MonitorConstants.CONFIG_ITEM+" = ?", new String[]{item});
		c.close();
		//updating config inside all instances
		//remove null pointer first;
		/*
		for(int i=0; i<mmVector.size();i++)
		{
			if(mmVector.get(i)==null)
			{
				mmVector.remove(i);
				i--;
			}else
			{
				try{
					//this will throw exception when the callback process terminates
					((IMonitorManagerCallback)(mmVector.get(i))).resynchronizeConfig(item);
				} catch(RemoteException e){
					mmVector.remove(i);
					i--;
					e.printStackTrace();
				}
			}

		}
		*/
		//if mode is changing, write the related info into LKM
		if(item.equals(MonitorConstants.CONFIG_MODE))
		{
			if(updateLKM(value)!=0)
				return -1;
		}
		return 0;
	}
	
	
	//@ policy by benjamin
	// update policy into LKM, the param policy is a string of package name + permission, with 2 digits for permission of SDcard and Internet access
	public int updateKernel() 
	{
		
		//int desc = PolicyManager.getDefault().PolicyOpen();
PolicyManager mypm = PolicyManager.getDefault();
System.out.println("daiting server my policymanager: "+mypm);
int desc = mypm.PolicyOpen();
		if(desc < 0)
		{
			Log.e(TAG, "daiting Device file cannot be opened");
			return -1;
		}

		//PolicyManager.getDefault().PolicyClearRules(desc);
System.out.println("daiting server permission before: "+mpermission + " " + mzname);//caller info
//parse permission for the final string, important
if (mpermission.contains(":")) {
	String part1;
	String part2;
	if (mpermission.contains("+")) {
		String[] strs = mpermission.split("\\+");
		String laststr = strs[strs.length-1];
		//can be configured to use the first none trebuchet activity
		/*String firststr = strs[0];
		if(firststr.contains("com.cyanogenmod.trebuchet")) {
			String[] parts = strs[1].split(":");
			part1 = parts[0];
			part2 = parts[1];
		} else {
			String[] parts = firststr.split(":");
			part1 = parts[0];
			part2 = parts[1];
		}*/
		String[] parts = laststr.split(":");
		part1 = parts[0];
		part2 = parts[1];
	} else {
		String[] parts = mpermission.split(":");
		part1 = parts[0];
		part2 = parts[1];
	}

mypm.PolicyAddRule(desc,   part1+"," + mpermission + "," + mzname);
int mynum = mypm.PolicyGetRuleNum(desc);
Log.e(TAG, "daiting server pname = "+ part1 + "; permission = " + mpermission + "; zone = " + mzname+ "; rule num= "+mynum);	
mypm.PolicyClose(desc);


} else {
throw new IllegalArgumentException("daiting mpermission=" + mpermission);
}




		
		/*Map res = getAllZone();
		Iterator iterator = res.entrySet().iterator();
		while (iterator.hasNext()) 
		{
			Map.Entry m = (Map.Entry)iterator.next();
			String zname = (String)m.getKey();
			String[] packages = getPackageFromZone(zname);
			String permission = ((String)m.getValue()).substring(11,13);
			
			for (int i = 0; i < packages.length; i++) 
			{
				PolicyManager.getDefault().PolicyAddRule(desc, packages[i] + "," + permission + "," + zname);
				Log.e(TAG, "Package = " + packages[i] + "; permission = " + permission + "; zone = " + zname);	
			}
		}*/
		
		/*String primary_external_storage_path = Environment.getExternalStorageDirectory().getAbsolutePath();		
		Log.e(TAG, "Primary External storage path = " + primary_external_storage_path);   
		
		String secondary_external_storage_path = System.getenv("SECONDARY_STORAGE");
		Log.e(TAG, "Secondary External storage path = " + secondary_external_storage_path); 
		
		PolicyManager.getDefault().PolicyExternalStorage(desc, primary_external_storage_path );
		
		PolicyManager.getDefault().PolicyClose(desc);*/
	
		return 0;
	}
	//# end by benjamin

	//update policy into LKM, the param mode is same with zone name currently
	private int updateLKM(String mode)
   	{
		/*int desc = SecEnvManager.getDefault().SecEnvOpen();
		if(desc < 0)
		{
			Log.e(TAG, "secenv LKM is not loaded");
			return -1;
		}
		SecEnvManager.getDefault().SecEnvClearRules(desc);
		//if mode set to "UnZone", just clear rules, otherwise fill with related packages
		if(!mode.equals("UnZone"))
		{
			String[] packages = getPackageFromZone(mode);
			for(int i = 0; i<packages.length; i++)
				SecEnvManager.getDefault().SecEnvAddRule(desc, packages[i]);
		}
		SecEnvManager.getDefault().SecEnvDump(desc);
		SecEnvManager.getDefault().SecEnvClose(desc);*/
		return 0;
	}

	/*
	// Executes UNIX command.
    private String exec(String command) {
        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            int read;
            char[] buffer = new char[4096];
            StringBuffer output = new StringBuffer();
            while ((read = reader.read(buffer)) > 0) {
                output.append(buffer, 0, read);
            }
            reader.close();
            process.waitFor();
            return output.toString();
        } catch (IOException e) {
            throw new RuntimeException(e);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
    */
}
