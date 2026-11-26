package com.hub7a4.ffmenu;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.widget.Toast;

public class MainActivity extends Activity {
    private static final int REQ_OVERLAY = 1001;
    private static final int REQ_DETAILS = 1002;
    private int deniedCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestRootShell();
        checkOverlayPermission();
    }

    private void requestRootShell() {
        try {
            Runtime.getRuntime().exec(new String[]{"su", "-c", "id"});
        } catch (Exception ignored) {}
    }

    private void checkOverlayPermission() {
        if (Settings.canDrawOverlays(this)) {
            startMenuService();
            return;
        }
        Intent intent = new Intent(
            Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
            Uri.parse("package:" + getPackageName())
        );
        intent.addFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);
        startActivityForResult(intent, REQ_OVERLAY);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQ_OVERLAY) {
            if (Settings.canDrawOverlays(this)) {
                startMenuService();
            } else if (++deniedCount < 2) {
                Toast.makeText(this, "Vui long cap quyen 'Hien thi tren ung dung khac'", Toast.LENGTH_SHORT).show();
                checkOverlayPermission();
            } else {
                Intent details = new Intent(
                    Settings.ACTION_APPLICATION_DETAILS_SETTINGS,
                    Uri.parse("package:" + getPackageName())
                );
                startActivityForResult(details, REQ_DETAILS);
            }
        } else if (requestCode == REQ_DETAILS) {
            if (Settings.canDrawOverlays(this)) {
                startMenuService();
            } else {
                finish();
            }
        }
    }

    private void startMenuService() {
        Intent svc = new Intent(this, OverlayService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(svc);
        } else {
            startService(svc);
        }
        finish();
    }
}
