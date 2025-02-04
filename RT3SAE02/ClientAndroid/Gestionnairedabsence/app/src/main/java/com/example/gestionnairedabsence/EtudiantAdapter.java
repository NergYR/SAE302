package com.example.gestionnairedabsence;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.BaseAdapter;

import java.util.List;

/**
 * Adaptateur personnalisé pour afficher une liste d'étudiants avec leur statut de présence.
 */
public class EtudiantAdapter extends BaseAdapter {
    private final Context context;
    private final List<Etudiant> etudiants;

    /**
     * Constructeur de l'adaptateur.
     * @param context Contexte de l'application
     * @param etudiants Liste des étudiants à afficher
     */
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
            holder.presenceIcon = convertView.findViewById(R.id.presenceIcon);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        // Masquer la première ligne (en-tête)
        if (position == 0) {
            convertView.setVisibility(View.GONE);
            convertView.setClickable(false);
            return convertView;
        }

        // Affichage des informations de l'étudiant
        convertView.setVisibility(View.VISIBLE);
        Etudiant etudiant = etudiants.get(position);
        holder.nomPrenomTextView.setText(etudiant.getNom() + " " + etudiant.getPrenom());

        // Mise à jour de l'icône selon la présence
        if (etudiant.getPresence() == 1) {
            holder.presenceIcon.setImageResource(android.R.drawable.checkbox_on_background);
        } else {
            holder.presenceIcon.setImageResource(android.R.drawable.checkbox_off_background);
        }

        // Gestion du clic pour alterner la présence
        convertView.setOnClickListener(v -> {
            int nouvellePresence = (etudiant.getPresence() == 0) ? 1 : 0;
            etudiant.setPresence(nouvellePresence);
            notifyDataSetChanged();
        });

        return convertView;
    }

    /**
     * Classe interne pour stocker les vues associées à chaque élément de la liste.
     */
    private static class ViewHolder {
        TextView nomPrenomTextView;
        ImageView presenceIcon;
    }
}
