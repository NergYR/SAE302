package com.example.gestionnairedabsence;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.TextView;

import java.util.List;

public class EtudiantAdapter extends BaseAdapter {
    private final Context context;
    private final List<Etudiant> etudiants;

    public EtudiantAdapter(Context context, List<Etudiant> etudiants) {
        this.context = context;
        this.etudiants = etudiants;
    }

    @Override
    public int getCount() {
        return etudiants.size();
    }

    @Override
    public Object getItem(int position) {
        return etudiants.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @SuppressLint("SetTextI18n")
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            convertView = LayoutInflater.from(context).inflate(R.layout.item_etudiant, parent, false);
            holder = new ViewHolder();
            holder.textNomPrenom = convertView.findViewById(R.id.textNomPrenom);
            holder.checkBoxPresent = convertView.findViewById(R.id.checkBoxPresent);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        final Etudiant etudiant = etudiants.get(position);
        holder.textNomPrenom.setText(etudiant.getNom() + " " + etudiant.getPrenom());
        holder.checkBoxPresent.setChecked(etudiant.isPresent());

        holder.checkBoxPresent.setOnCheckedChangeListener((buttonView, isChecked) -> etudiant.setPresent(isChecked));

        return convertView;
    }

    static class ViewHolder {
        TextView textNomPrenom;
        CheckBox checkBoxPresent;
    }
}

