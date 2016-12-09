// Hubert Jasudowicz, Konrad Majewski
#ifndef VIRUS_GENEALOGY_H_
#define VIRUS_GENEALOGY_H_

#include <exception>

class VirusAlreadyCreated : public std::exception {}

class VirusNotFound : public std::exception {}

class TriedToRemoveStemVirus : public std::exception {}

template <typename Virus>
class VirusGenealogy {
  public:
    VirusGenealogy(Virus::id_type const &stem_id);
    
    Virus::id_type get_stem_id() const;
    
    std::vector<Virus::id_type> get_children(Virus::id_type const &id) const;
    
    std::vector<Virus::id_type> get_parents(Virus::id_type const &id) const;
    
    bool exists(Virus::id_type const &id) const;
    
    Virus& operator[](Virus::id_type const &id) const;
    
    void create(Virus::id_type const &id, Virus::id_type const &parent_id);
    
    void create(Virus::id_type const &id, std::vector<Virus::id_type> const &parent_ids);
    
    void connect(Virus::id_type const &child_id, virus::id_type const &parent_id);
    
    void remove(Virus::id_type const &id);
};

#endif //VIRUS_GENEALOGY_H_
