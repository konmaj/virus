// Hubert Jasudowicz, Konrad Majewski
#ifndef VIRUS_GENEALOGY_H_
#define VIRUS_GENEALOGY_H_

#include <exception>

class VirusAlreadyCreated : public std::exception {
  public:
    const char* what() const {
        return "VirusAlreadyCreated";
    }
}

class VirusNotFound : public std::exception {
  public:
    const char* what() const {
        return "VirusNotFound";
    }
}

class TriedToRemoveStemVirus : public std::exception {
  public:
    const char* what() const {
        return "TriedToRemoveStemVirus";
    }
}

template <typename Virus>
class VirusGenealogy {
  private:
    Virus::id_type stem_id_;
    
  public:
    VirusGenealogy(Virus::id_type const &stem_id);
    
    VirusGenealogy(const VirusGenealogy<Virus> &other) = delete;
    
    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus> &other) = delete;
    
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
