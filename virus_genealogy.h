// Hubert Jasudowicz, Konrad Majewski
#ifndef VIRUS_GENEALOGY_H_
#define VIRUS_GENEALOGY_H_

#include <exception>
#include <map>
#include <memory>
#include <vector>
#include <set>

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
    VirusGenealogy(id_type const &stem_id)
        : stem_node_(std::make_shared<Node>(*this, stem_id)),
          nodes_(std::map<id_type, std::weak_ptr<Node>>{{stem_id, stem_node_}}) {
        stem_node_->position = nodes_.begin();
    }

    VirusGenealogy(const VirusGenealogy<Virus> &other) = delete;

    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus> &other) = delete;

    ~VirusGenealogy() {
        stem_node_.reset();
    }

    id_type get_stem_id() const {
        return (stem_node_->virus).get_id();
    }

    bool exists(id_type const &id) const {
        return nodes_.find(id) != nodes_.end();
    }

    std::vector<id_type> get_children(id_type const &id) const {
        throw_if_not_exists(id);

        std::shared_ptr<Node> node = (nodes_.find(id)->second).lock();

        // Reference to vector of shared pointers to children
        const auto& children_nodes = node->children;

        std::vector<id_type> children_ids;
        for (auto node : children_nodes) {
            children_ids.emplace_back(node->virus.get_id());
        }

        return children_ids;
    }

    std::vector<id_type> get_parents(id_type const &id) const {
        throw_if_not_exists(id);

        std::shared_ptr<Node> node_ptr = (nodes_.find(id)->second).lock();
        // Reference to vector of weak pointers to parents
        auto& node_parents = node_ptr->parents;

        std::vector<id_type> parent_ids;
        for (auto node : node_parents) {
            std::shared_ptr<Node> parent = node.lock();
            parent_ids.emplace_back(parent->virus.get_id());
        }

        return parent_ids;
    }

    Virus& operator[](id_type const &id) const {
        throw_if_not_exists(id);
        return (nodes_.find(id)->second).lock()->virus;
    }

    void create(id_type const &id, id_type const &parent_id) {
        create(id, std::vector<id_type>{parent_id});
    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (exists(id)) {
            throw VirusAlreadyCreated();
        }
        
        // Virus must have at least one parent
        if (parent_ids.empty()) {
            throw VirusNotFound();
        }

        for (const auto& parent : parent_ids) {
            throw_if_not_exists(parent);
        }

        std::shared_ptr<Node> new_node = std::make_shared<Node>(*this, id);

        // Find pointers to nodes from parent ids.
        std::vector<std::shared_ptr<Node>> parents;
        for (auto id : parent_ids) {
            auto ptr = (nodes_.find(id)->second).lock();
            parents.emplace_back(ptr);
        }

        // Clone all children sets
        std::vector<std::set<std::shared_ptr<Node>>> parent_children_copy;
        for (auto parent : parents) {
            auto children_copy = parent->children;
            parent_children_copy.emplace_back(children_copy);
        }

        // Add child to each children set
        for (auto& children : parent_children_copy) {
            children.insert(new_node);
        }

        // Add parents to parents set
        for (auto& parent : parents) {
            (new_node->parents).insert(parent);
        }

        // Copying went ok, so we can add new node to map
        auto map_it = nodes_.insert(std::make_pair(id, new_node)).first;
        new_node->position = map_it;

        for (size_t i = 0; i < parents.size(); i++) {
            (parents[i]->children).swap(parent_children_copy[i]);
        }
    }

    void connect(id_type const &child_id, id_type const &parent_id) {
        throw_if_not_exists(child_id);
        throw_if_not_exists(parent_id);

        std::shared_ptr<Node> child = (nodes_.find(child_id)->second).lock();
        std::shared_ptr<Node> parent = (nodes_.find(parent_id)->second).lock();
        auto parents_copy = child->parents;
        auto children_copy = parent->children;

        parents_copy.insert(std::weak_ptr<Node>(parent));
        children_copy.insert(std::shared_ptr<Node>(child));

        (child->parents).swap(parents_copy);
        (parent->children).swap(children_copy);
    }

    void remove(id_type const &id) {
        throw_if_not_exists(id);

        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirus();
        }
        
        std::weak_ptr<Node> weak_node = nodes_.find(id)->second;
        std::shared_ptr<Node> node = weak_node.lock();
        std::vector<std::set<std::shared_ptr<Node>>> parent_children_copy;
        std::vector<std::set<std::weak_ptr<Node>, 
                             std::owner_less<std::weak_ptr<Node>>>> 
                                  child_parents_copy;

        for (auto& parent : node->parents) {
            parent_children_copy.emplace_back(parent.lock()->children);
        }
        
        for (auto& children : parent_children_copy) {
            children.erase(node);
        }
        
        for (auto& child : node->children) {
            child_parents_copy.emplace_back(child->parents);
        }
        
        for (auto& parents : child_parents_copy) {
            parents.erase(weak_node);
        }
        
        size_t index = 0;
        for (auto& parent : node->parents) {
            (parent.lock()->children).swap(parent_children_copy[index]);
            index++;
        }
        
        index = 0;
        for (auto& child : node->children) {
            (child->parents).swap(child_parents_copy[index]);
            index++;
        }
    }

private:
    // Root virus node
    std::shared_ptr<Node> stem_node_;

    std::map<id_type, std::weak_ptr<Node>> nodes_;

    struct Node {
        std::set<std::shared_ptr<Node>> children;
        std::set<std::weak_ptr<Node>,
                 std::owner_less<std::weak_ptr<Node>>> parents;
        VirusGenealogy<Virus>& genealogy;
        typename std::map<id_type, std::weak_ptr<Node>>::iterator position;

        Virus virus;

        Node(VirusGenealogy<Virus>& virusGenealogy, id_type id) : genealogy(virusGenealogy), virus(id) {}

        ~Node() {
            genealogy.nodes_.erase(position);
        }
    };

    void throw_if_not_exists(id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
    }

};

#endif //VIRUS_GENEALOGY_H_