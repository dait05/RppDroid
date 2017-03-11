package swl.monitor;

import java.util.Map;
//import IMonitorManagerCallback;

/**
 * 
 * @author xiaolei
 *
 */

/** {@hide} */

interface IMonitorManager
{
	int checkPolicy(String packagename);
	boolean getPolicy(int POLICY_CATEGORY, String packagename);
	int setPolicy(int POLICY_CATEGORY, String packagename, boolean policy);
	
	int setZone(String zname, String permission);
	Map getAllZone();
	String getPermissionFromZone(String zname);
	String getZoneFromPackage(String pname);
	Map getAllPackageZone();
	String[] getPackageFromZone(String zname);
	int setPackageZone(String pname, String zname);

	String getConfigItem(String item);
	int setConfigItem(String item, String value);
	
	
	//@ policy by benjamin
	int updateKernel();
	//#end by benjamin
}

