package android.provider;

import android.net.Uri;

public class MonitorConstants {
	public static final String DATABASE_NAME = "monitordatastorage";
	public static final int DATABASE_VERSION=1;

	public static final String TABLE_CONFIG="table_config";

	public static final String TABLE_ZONE="table_zone_permission";
	public static final String TABLE_PACKAGE="table_package_zone";

	public static final String TABLE_ACCOUNT="behavior_account";
	public static final String TABLE_CALENDAR="behavior_calendar";
	public static final String TABLE_CAMERA="behavior_camera";
	public static final String TABLE_CONTACT="behavior_contact";
	public static final String TABLE_HISTORY_BOOKMARK="behavior_history_bookmark";
	public static final String TABLE_LOCATION="behavior_location";
	public static final String TABLE_LOG="behavior_log";
	public static final String TABLE_MICROPHONE="behavior_microphone";
	public static final String TABLE_PHONE_STATE="behavior_phone_state";
	public static final String TABLE_SMS="behavior_sms";
	public static final String TABLE_SUBSCRIBED_FEED="behavior_subscribed_feed";
	public static final String TABLE_SDCARD="behavior_sdcard";
	public static final String TABLE_INTERNET="behavior_internet";
	public static final String TABLE_ADMIN="behavior_admin";

	
	public static final String CONFIG_ITEM="config_item";
	public static final String CONFIG_VALUE="config_value";

	public static final String CONFIG_ENABLE="config_enable";
	public static final String CONFIG_MODE="config_mode";

	public static final String ZONE_NAME="zname";
	public static final String PERMISSION="perm";
	public static final String PACKAGE_NAME="pname";
	public static final String ALLOW_DENY="adchoice"; //0 deny, 1 allow, default 1

	public static final String KEY_ID="_id";

	public static final String AUTHORITY = "com.android.providers.monitor";
	public static final Uri AUTHORITY_URI = Uri.parse("content://" + AUTHORITY);

	public static final int CONFIG_POLICY=0;
	public static final class CONFIG {
		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_CONFIG);
	}

	public static final int ZONE_POLICY=1;
	public static final class ZONE {
		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_ZONE);
	}

	public static final int PACKAGE_POLICY=2;
	public static final class PACKAGE {
		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_PACKAGE);
	}

	public static final int ACCOUNT_POLICY=3;
	public static final class ACCOUNT {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_ACCOUNT);

	}

	public static final int CALENDAR_POLICY=4;
	public static final class CALENDAR {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_CALENDAR);

	}

	public static final int CAMERA_POLICY=5;
	public static final class CAMERA {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_CAMERA);

	}

	public static final int CONTACT_POLICY=6;
	public static final class CONTACT {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_CONTACT);

	}

	public static final int HISTORY_BOOKMARK_POLICY=7;
	public static final class HISTORY_BOOKMARK {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_HISTORY_BOOKMARK);

	}

	public static final int LOCATION_POLICY=8;
	public static final class LOCATION {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_LOCATION);

	}

	public static final int LOG_POLICY=9;
	public static final class LOG {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_LOG);

	}

	public static final int MICROPHONE_POLICY=10;
	public static final class MICROPHONE {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_MICROPHONE);

	}

	public static final int PHONE_STATE_POLICY=11;
	public static final class PHONE_STATE {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_PHONE_STATE);

	}
	
	public static final int SMS_POLICY=12;
	public static final class SMS {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_SMS);

	}

	public static final int SUBSCRIBED_FEED_POLICY=13;
	public static final class SUBSCRIBED_FEED {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_SUBSCRIBED_FEED);

	}
	
	public static final int SDCARD_POLICY=14;
	public static final class SDCARD {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_SDCARD);

	}

	public static final int INTERNET_POLICY=15;
	public static final class INTERNET {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_INTERNET);

	}
	public static final int ADMIN_POLICY=16;
	public static final class ADMIN {

		public static final Uri CONTENT_URI = Uri.withAppendedPath(AUTHORITY_URI, TABLE_ADMIN);

	}

}
