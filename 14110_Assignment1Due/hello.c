#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>

#include <linux/ip.h>
#include <linux/tcp.h>
#undef __KERNEL__
#include <linux/netfilter_ipv4.h>
#define __KERNEL__
static struct nf_hook_ops nfho;

unsigned int hook_func( unsigned int hooknum,
                    struct sk_buff * skb,
                    const struct net_device * in,
                    const struct net_device * out,
                    int (*okfn)(struct sk_buff *))
{
    struct iphdr    * iph;
    struct tcphdr   * tcph;
    unsigned char   * http_port = "\x00\x50";
    char            * data;

    if (skb)
    {
        iph = ip_hdr(skb);

        if (iph && iph->protocol && (iph->protocol == IPPROTO_TCP))
        {
            tcph = (struct tcphdr *)((__u32 *)iph + iph->ihl);
            if(tcph->syn || tcph->fin || tcph->urg || tcph->ack || tcph->psh) {
                data    = (char *)((unsigned char *)tcph + (tcph->doff * 4));

                /*printk(KERN_INFO "TCP source : %hu, TCP  dest : %hu\n", ntohs(tcph->source), ntohs(tcph->dest));
                printk(KERN_INFO "TCP seq : %u, TCP ack_seq : %u\n", ntohl(tcph->seq), ntohl(tcph->ack_seq));
                printk(KERN_INFO "TCP doff : %hu, TCP window : %hu\n", ntohs(tcph->doff), ntohs(tcph->window));
                printk(KERN_INFO "TCP check : 0x%hx, TCP urg_ptr : %hu\n", ntohs(tcph->check), ntohs(tcph->urg_ptr));
                */
                char flag[10];
                if (tcph->urg) {
                	strcpy(flag, "URG");
                } else if (tcph->ack) {
                	strcpy(flag, "ACK");
                } else if (tcph->psh) {
                	strcpy(flag, "PSH");
                } else if (tcph->syn) {
                	strcpy(flag, "SYN");
                } else if (tcph->fin) {
                	strcpy(flag, "FIN");
                }

                printk(KERN_INFO " FLAGS = %s\n\n", flag);
                //printk(KERN_DEBUG "sending packet to : %pI4\n", &iph->daddr);
/*                printk(KERN_DEBUG "data len : %d\ndata : \n", (int) strlen(data));
                printk(KERN_DEBUG "%s\n", data);*/
        }
    }
    }

    return NF_ACCEPT;
}

int init_module()
{
    int result;

    nfho.hook   = (nf_hookfn *) hook_func;
    nfho.hooknum    = NF_IP_POST_ROUTING;
    nfho.pf     = PF_INET;
    nfho.priority   = NF_IP_PRI_FIRST;

    result = nf_register_hook(&nfho);

    if(result)
    {
        printk(KERN_DEBUG "firewall : erreur nf_register_hook !\n");
        return 1;
    }

    printk(KERN_DEBUG "firewall : module charge.\n");

    return 0;
}

void cleanup_module()
{
    nf_unregister_hook(&nfho);
    printk(KERN_DEBUG "firewall : module decharge.\n");
}