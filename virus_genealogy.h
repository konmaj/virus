// Hubert Jasudowicz, Konrad Majewski
#ifndef VIRUS_GENEALOGY_H_
#define VIRUS_GENEALOGY_H_

#include <exception>
#include <map>
#include <memory>
#include <vector>

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

    struct Node;
    // Internal type for Virus IDs
    using id_type = typename Virus::id_type;

    // Root virus node
    std::shared_ptr<Node> stem_node_;

    typename std::map<id_type, std::weak_ptr<Node>> nodes_;

    struct Node {
        std::vector<std::shared_ptr<Node>> children;
        std::vector<std::weak_ptr<Node>> parents;

        typename std::map<id_type, std::weak_ptr<Node>>::iterator position;

        Virus virus;

        // TODO (constructor)
        ~Node() {
            nodes_.erase(position);
        }
    };

public:
    VirusGenealogy(id_type const &stem_id) { // Hubert

    }

    VirusGenealogy(const VirusGenealogy<Virus> &other) = delete;

    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus> &other) = delete;

    id_type get_stem_id() const {
        return (stem_node_->virus).get_id();
    }

    bool exists(id_type const &id) const {
        return nodes_.find(id) != nodes_.end();
    }

    std::vector<id_type> get_children(id_type const &id) const {} // Hubert

    std::vector<id_type> get_parents(id_type const &id) const {} // Hubert

    Virus& operator[](id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }

        return (nodes_.find(id)->second).lock()->virus;
    }

    void create(id_type const &id, id_type const &parent_id) {
        if (exists(id)) {
            throw VirusAlreadyCreated();
        }

        if (!exists(parent_id)) {
            throw VirusNotFound();
        }

    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (exists(id)) {
            throw VirusAlreadyCreated();
        }

        for (const auto& parent : parent_id) {
            if(!exists(parent)) {
                throw VirusNotFound();
            }
        }
    }

    void connect(id_type const &child_id, id_type const &parent_id) {
        if (!exists(child_id) || !exists(parent_id)) {
            throw VirusNotFound();
        }

        std::shared_ptr<Node> child_ptr = (nodes_.find(child_id)->second).lock();
        std::shared_ptr<Node> parent_ptr = (nodes_.find(parent_id)->second).lock();
        auto parents_copy = child_ptr->parents;
        auto children_copy = parent_ptr->children;

        parents_copy.emplace_back(std::weak_ptr<Node>(parent_ptr));
        children_copy.emplace_back(std::shared_ptr<Node>(child_ptr));

        (child_ptr->parents).swap(parents_copy);
        (parent_ptr->children).swap(children_copy);
    }

    void remove(id_type const &id) {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirus();
        }

        std::shared_ptr<Node> node_ptr = (nodes_.find(id)->second).lock();
        std::vector<std::vector<std::shared_ptr<Node>>> children_copies(node_ptr->parents.size());
        // TODO (copy vectors of children and erase id from them)
    }
};

#endif //VIRUS_GENEALOGY_H_
