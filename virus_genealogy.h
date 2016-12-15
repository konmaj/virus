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

template <class Virus>
class VirusGenealogy {

    struct Node;

    // Internal type for Virus IDs
    using id_type = typename Virus::id_type;

public:
    VirusGenealogy(id_type const &stem_id) { // can throw bad_alloc
        std::shared_ptr<Node> new_stem_node = std::make_shared<Node>(stem_id);

        stem_node_ = new_stem_node;

        auto map_it = nodes_.insert(std::make_pair(stem_id, new_stem_node)).first;
        new_stem_node->position = map_it;
    }

    VirusGenealogy(const VirusGenealogy<Virus> &other) = delete;

    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus> &other) = delete;

    id_type get_stem_id() const {
        return (stem_node_->virus).get_id();
    }

    bool exists(id_type const &id) const {
        return nodes_.find(id) != nodes_.end();
    }

    std::vector<id_type> get_children(id_type const &id) const {
        throw_if_not_exists(id);

        std::shared_ptr<Node> node = (nodes_.find(id)->second).lock();
        const auto& children_nodes = node->children; //vector of shared pointers to children

        std::vector<id_type> ret;
        for (auto node : children_nodes) {
            ret.emplace_back(node->virus.get_id());
        }

        return ret;
    }

    std::vector<id_type> get_parents(id_type const &id) const {
        throw_if_not_exists(id);

        std::shared_ptr<Node> node = (nodes_.find(id)->second).lock();
        const auto& parent_nodes = node->parents; //vector of weak pointers to parents

        std::vector<id_type> ret;
        for (auto node : parent_nodes) {
            std::shared_ptr<Node> parent = node.lock();
            ret.emplace_back(parent->virus.get_id());
        }

        return ret;
    }

    Virus& operator[](id_type const &id) const {
        throw_if_not_exists(id);
        return (nodes_.find(id)->second).lock()->virus;
    }

    void create(id_type const &id, id_type const &parent_id) {
        if (exists(id)) {
            throw VirusAlreadyCreated();
        }

        throw_if_not_exists(parent_id);

        std::shared_ptr<Node> new_node = std::make_shared<Node>(id);
        auto map_it = nodes_.insert(std::make_pair(id, new_node)).first;

        try {
            new_node->position = map_it;
            connect(id, parent_id);
        } catch(...) {
            new_node->position = nodes_.end();
            nodes_.erase(map_it);
        }

    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (exists(id)) {
            throw VirusAlreadyCreated();
        }

        for (const auto& parent : parent_ids) {
            throw_if_not_exists(parent);
        }

        std::shared_ptr<Node> new_node = std::make_shared<Node>(id);
        auto map_it = nodes_.insert(std::make_pair(id, new_node)).first;

        try {
            new_node->position = map_it;
            
            for (auto parent : parent_ids) {
                connect(id, parent);
            }

        } catch (...) {
            new_node->position = nodes_.end();
            nodes_.erase(map_it);
            // TODO rollback parents
        }
    }

    void connect(id_type const &child_id, id_type const &parent_id) {
        throw_if_not_exists(child_id);
        throw_if_not_exists(parent_id);

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
        throw_if_not_exists(id);

        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirus();
        }

        std::shared_ptr<Node> node_ptr = (nodes_.find(id)->second).lock();
        std::vector<std::vector<std::shared_ptr<Node>>> children_copies(node_ptr->parents.size());
        // TODO (copy vectors of children and erase id from them)
    }

private:
    // Root virus node
    std::shared_ptr<Node> stem_node_;

    typename std::map<id_type, std::weak_ptr<Node>> nodes_;

    struct Node {
        std::vector<std::shared_ptr<Node>> children;
        std::vector<std::weak_ptr<Node>> parents;

        typename std::map<id_type, std::weak_ptr<Node>>::iterator position;

        Virus virus;

        Node(id_type id) : virus(id) {}
    };

    void throw_if_not_exists(id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
    }

};

#endif //VIRUS_GENEALOGY_H_
