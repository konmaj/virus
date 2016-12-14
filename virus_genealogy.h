// Hubert Jasudowicz, Konrad Majewski
#ifndef VIRUS_GENEALOGY_H_
#define VIRUS_GENEALOGY_H_

#include <exception>
#include <map>
#include <vector>
#include <memory>

class VirusAlreadyCreated : public std::exception {
  public:
    const char* what() const noexcept {
        return "VirusAlreadyCreated";
    }
};

class VirusNotFound : public std::exception {
  public:
    const char* what() const noexcept {
        return "VirusNotFound";
    }
};

class TriedToRemoveStemVirus : public std::exception {
  public:
    const char* what() const noexcept {
        return "TriedToRemoveStemVirus";
    }
};

template <typename Virus>
class VirusGenealogy {
private:

    class Node;
    // Internal type for Virus IDs
    using id_type = typename Virus::id_type;
    using Node_ptr = std::shared_ptr<Node>;

    // Root virus ID
    id_type stem_id_;

    std::map<id_type, Node_ptr> nodes_;

    class Node {
        std::vector<Node_ptr> parents;
        std::vector<Node_ptr> children;
    };

public:
    VirusGenealogy(id_type const &stem_id) {

    }

    VirusGenealogy(const VirusGenealogy<Virus> &other) = delete;

    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus> &other) = delete;

    id_type get_stem_id() const {
        return stem_id_;
    }

    std::vector<id_type> get_children(id_type const &id) const {}

    std::vector<id_type> get_parents(id_type const &id) const {}

    bool exists(id_type const &id) const {}

    Virus& operator[](id_type const &id) const {}

    void create(id_type const &id, id_type const &parent_id) {}

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {}

    void connect(id_type const &child_id, id_type const &parent_id) {}

    void remove(id_type const &id) {}
};

#endif //VIRUS_GENEALOGY_H_
