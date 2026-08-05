#include <pqxx/pqxx>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <optional>

struct Client
{
    int id;
    std::string first_name;
    std::string last_name;
    std::string email;
    std::vector<std::string> phone;
};

class ClientManager
{


private:
    pqxx::connection& connection;

public:
    ClientManager(pqxx::connection& conn) : connection(conn) 
    {
        prepareStatements();
    }

private:
    void prepareStatements()
    {
        pqxx::work tx(connection);
        tx.conn().prepare("create_clients", R"SQL(
            CREATE TABLE IF NOT EXISTS clients (
                id SERIAL PRIMARY KEY,
                first_name TEXT NOT NULL,
                last_name TEXT NOT NULL,
                email TEXT NOT NULL UNIQUE,
                created_at TIMESTAMPTZ NOT NULL DEFAULT now()
            )
        )SQL");

        tx.conn().prepare("create_phone", R"SQL(
            CREATE TABLE IF NOT EXISTS phone (
                id SERIAL PRIMARY KEY,
                client_id INTEGER NOT NULL REFERENCES clients(id) ON DELETE CASCADE,
                phone_number TEXT NOT NULL,
                UNIQUE (client_id, phone_number)
            )
        )SQL");

        tx.conn().prepare("add_client", "INSERT INTO clients (first_name, last_name, email) VALUES ($1, $2, $3) RETURNING id");
        tx.conn().prepare("add_phone", "INSERT INTO phone (client_id, phone_number) VALUES ($1, $2)");
        tx.conn().prepare("update_client", "UPDATE clients SET first_name = $1, last_name = $2, email = $3 WHERE id = $4");
        tx.conn().prepare("delete_phone", "DELETE FROM phone WHERE client_id = $1 AND phone_number = $2");
        tx.conn().prepare("delete_client", "DELETE FROM clients WHERE id = $1");
        


        tx.conn().prepare("find_clients", R"SQL(
            SELECT DISTINCT c.id, c.first_name, c.last_name, c.email, p.phone_number
            FROM clients c
            LEFT JOIN phone p ON c.id = p.client_id
            WHERE c.first_name ILIKE $1
               OR c.last_name ILIKE $1
               OR c.email ILIKE $1
               OR p.phone_number ILIKE $1
            ORDER BY c.id
        )SQL");


        tx.commit();
    }

public:
    void createTable()
    {
        pqxx::work tx(connection);
        tx.exec_prepared("create_clients");
        tx.exec_prepared("create_phone");
        tx.commit();
    }

    int addClient(const std::string& first_name, const std::string& last_name, const std::string& email)
    {
        pqxx::work tx(connection);
        pqxx::result res = tx.exec_prepared("add_client", first_name, last_name, email);
        tx.commit();
        return res[0][0].as<int>();
    }

    void addPhone(int client_id, const std::string& phone)
    {
        pqxx::work tx(connection);
        tx.exec_prepared("add_phone", client_id, phone);
        tx.commit();
    }

    void updateClient(int client_id, const std::string& first_name, const std::string& last_name, const std::string& email)
    {
        pqxx::work tx(connection);
        tx.exec_prepared("update_client", first_name, last_name, email, client_id);
        tx.commit();
    }

    void deletePhone(int client_id, const std::string& phone)
    {
        pqxx::work tx(connection);
        tx.exec_prepared("delete_phone", client_id, phone);
        tx.commit();
    }

    void deleteClient(int client_id)
    {
        pqxx::work tx(connection);
        tx.exec_prepared("delete_client", client_id);
        tx.commit();
    }

    std::vector<Client> findClients(const std::string& query_str) const
    {
        pqxx::read_transaction tx(connection);
        std::string search_pattern = "%" + query_str + "%";
        pqxx::result res = tx.exec_prepared("find_clients", search_pattern);

        std::vector<Client> result;
        int last_id = -1;
        Client current;

        for (const auto& row : res) {
            int id = row["id"].as<int>();
            if (id != last_id)
            {
                if (last_id != -1)
                    result.push_back(std::move(current));
                
                current.id = id;
                current.first_name = row["first_name"].as<std::string>();
                current.last_name = row["last_name"].as<std::string>();
                current.email = row["email"].as<std::string>();
                current.phone.clear();
                last_id = id;
            }
            
            if (!row["phone_number"].is_null())
                current.phone.push_back(row["phone_number"].as<std::string>());
        }
        
        if (last_id != -1)
            result.push_back(std::move(current));

        return result;
    }

};

namespace
{
std::string read_env_or_default(const char* name, const char* default_value) {
if (const char* value = std::getenv(name); value != nullptr && value[0] != '\0') {
return value; }
return default_value; }
std::string build_connection_string() {
const std::string host = read_env_or_default("PGHOST", "localhost");
const std::string port = read_env_or_default("PGPORT", "5432");
const std::string dbname = read_env_or_default("PGDATABASE", "cpp_demo"); const std::string user = read_env_or_default("PGUSER", "postgres");
const std::string password = read_env_or_default("PGPASSWORD", "postgres");
return
"host=" + host + " " +
"port=" + port + " " + "dbname=" + dbname + " " + "user=" + user + " " + "password=" + password;
}
} // namespace


int main()
{
    try
    {
        pqxx::connection connection{build_connection_string()};
        std::cout << "Connected to database: " << connection.dbname() << '\n';

        ClientManager manager(connection);
        manager.createTable();
        std::cout << "Tables Ok\n";

        int id1 = manager.addClient("Ivan", "Petrov", "ivan@example.com");
        int id2 = manager.addClient("Maria", "Ivanova", "maria@example.com");
        std::cout << "Add client Ok " << id1 << ", " << id2 << '\n';

        manager.addPhone(id1, "+71234567890");
        manager.addPhone(id1, "+79876543210");
        manager.addPhone(id2, "+71112223344");
        std::cout << "Phone add Ok.\n";

        manager.updateClient(id1, "Ivan", "Ivaonv", "ivan.new@example.com");
        std::cout << "Updat client Ol " << id1 << ".\n";

        auto found_by_name = manager.findClients("Ivan");
        std::cout << "\nSearch by 'Ivan':\n";
        for (const auto& c : found_by_name)
        {
            std::cout << "ID: " << c.id << ", " << c.first_name << " " << c.last_name
                      << ", email: " << c.email << ", phones: ";
            if (c.phone.empty()) std::cout << "(none)";
            else for (const auto& p : c.phone) std::cout << p << " ";
            std::cout << '\n';
        }

        auto find_phone = manager.findClients("1112223344");
        std::cout << "\nSearch by part of phone '1112223344':\n";
        for (const auto& c : find_phone)
        {
            std::cout << "ID: " << c.id << ", " << c.first_name << " " << c.last_name
                      << ", email: " << c.email << ", phones: ";
            for (const auto& p : c.phone) std::cout << p << " ";
            std::cout << '\n';
        }

        manager.deletePhone(id1, "+79876543210");
        std::cout << "\n Delet phone Ok " << id1 << ".\n";

        manager.deleteClient(id2);
        std::cout << "Delet client Ok " << id2 << ".\n";

        auto all = manager.findClients("Ivan");
        std::cout << "\n Clients after delet\n";
        for (const auto& c : all)
        {
            std::cout << "ID: " << c.id << ", " << c.first_name << " " << c.last_name
                      << ", email: " << c.email << ", phones: ";
            if (c.phone.empty()) std::cout << "(none)";
            else for (const auto& p : c.phone) std::cout << p << " ";
            std::cout << '\n';
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}