package com.trans;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import java.io.File;
import java.util.List;

public class SetActivity$PopFragment extends DialogFragment {
    public SetActivity$PopFragment() {
    }

    public void onActivityCreated(Bundle p0) {
        super.onActivityCreated(p0);
    }

    public Dialog onCreateDialog(Bundle p0) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Select Directory");
        builder.setMessage("Choose a directory");
        builder.setPositiveButton("OK", null);
        builder.setNegativeButton("Cancel", null);
        return builder.create();
    }

    public View onCreateView(LayoutInflater p0, ViewGroup p1, Bundle p2) {
        ListView listView = new ListView(getActivity());
        List<File> dirs = SetActivity.findNumDirs(
            new File(getActivity().getExternalFilesDir(null), "userdata")
        );
        String[] items = new String[dirs.size()];
        for (int i = 0; i < dirs.size(); i++) {
            items[i] = dirs.get(i).getName();
        }
        ArrayAdapter<String> adapter = new ArrayAdapter<>(getActivity(),
            android.R.layout.simple_list_item_1, items);
        listView.setAdapter(adapter);
        return listView;
    }
}
