package com.example.gestionnairedabsence;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.BaseAdapter;

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

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            LayoutInflater inflater = LayoutInflater.from(context);
            convertView = inflater.inflate(R.layout.item_etudiant, parent, false);

            holder = new ViewHolder();
            holder.nomPrenomTextView = convertView.findViewById(R.id.nomPrenomTextView);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        Etudiant etudiant = etudiants.get(position);
        holder.nomPrenomTextView.setText(etudiant.getNom() + " " + etudiant.getPrenom());

        return convertView;
    }

    private static class ViewHolder {
        TextView nomPrenomTextView;
    }
}
